/* Copyright 2014, Brown University, Providence, RI.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose other than its incorporation into a
 * commercial product is hereby granted without fee, provided that the
 * above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Brown University not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 * BROWN UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file searchlight.h
 *
 * This is the main entry point for the search process. It contains a number of
 * tools to guide and monitor search, and also provides a number of other
 * useful tools, like estimation API.
 *
 * @author Alexander Kalinin
 */

#ifndef SEARCHLIGHT_SEARCHLIGHT_H_
#define SEARCHLIGHT_SEARCHLIGHT_H_

#include "ortools_inc.h"
#include "array_desc.h"

#include <system/Config.h>

#include <dlfcn.h>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <thread>
#include <mutex>

namespace searchlight {

/**
 * Property tree used to store configuration options.
 */
using SearchlightConfig = boost::property_tree::ptree;

class Validator;
class SearchlightCollector;
class SearchlightTask;

/**
 * This class manages supplementary DLL resources required by Searchlight.
 * It is guaranteed to close all opened libraries at the end, on destruction.
 * It also serves as a map of names->dll handles, thus allowing retrieval of
 * DLLs via names, avoiding duplicate open calls.
 *
 * The need of such a manager is also emphasized by the necessity of closing
 * the libraries at the very end. For example, if you create an object with
 * the definition in a DLL and later try to delete it after closing the
 * library, you will have trouble with calling the destructor. That is why
 * the handler should be deleted at the very end.
 */
class DLLHandler : private boost::noncopyable {
public:
    /**
     * Constructs a new DLL handler. It assumes the default DLL directory
     * to be the SciDb plugin's directory.
     */
    DLLHandler() :
        dlls_dir_(scidb::Config::getInstance()->
                getOption<std::string>(scidb::CONFIG_PLUGINS)) {}

    /**
     * Destructor. Closes all opened DLLs.
     */
    ~DLLHandler() {
        for (auto &name_lib: dlls_) {
            dlclose(name_lib.second);
        }

    }

    /**
     * Loads the specified DLL into memory. If the library was loaded before,
     * it returns the same handle. If the loading is impossible for some
     * reason, it throws a SciDb system exception.
     *
     * @param name the name of the library, without the suffix or prefix
     * @return the DLL handle (never nullptr)
     */
    void *LoadDLL(const std::string &name) {
        auto it = dlls_.find(name);
        if (it == dlls_.end()) {
            std::string full_name = dlls_dir_ + "/lib" + name + ".so";
            void *dll_handle= dlopen(full_name.c_str(), RTLD_LAZY | RTLD_LOCAL);
            if (!dll_handle) {
                std::ostringstream err_msg;
                err_msg << "Cannot load the task library: name=" <<
                        full_name << ", reason=" << dlerror();
                throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR,
                        SCIDB_LE_ILLEGAL_OPERATION) << err_msg.str();
            }

            dlls_[name] = dll_handle;
            return dll_handle;
        } else {
            return it->second;
        }
    }

private:
    // Default DLL directory
    std::string dlls_dir_;

    // Map: DLL name -> DLL handle
    std::map<std::string, void *> dlls_;
};

/**
 * This is a monitor that catches complete, but approximate, solutions
 * (assignments) and passed them along to the Validator.
 */
class ValidatorMonitor : public SolutionCollector {
public:
    /**
     * Creates a new validator monitor. This monitor looks for complete
     * assignments during the search and passed them along to the validator.
     *
     * @param validator the validator for checking assignments
     * @param vars a vector of decision variables (externally managed)
     * @param solver the main solver
     */
    ValidatorMonitor(Validator &validator, const IntVarVector &vars,
            Solver *solver) :
        SolutionCollector(solver),
        validator_(validator),
        vars_(vars) {
        Add(vars);
    }

    /**
     * This function is called at a leaf of the search tree. At this point
     * a leaf is accepted as being a solution. This function checks if
     * it is a complete assignment and passed it along to the validator.
     *
     * @return true if we want to continue after the leaf; false otherwise
     */
    virtual bool AtSolution();

    /**
     * Returns the number of candidates encountered. This monitor tracks them,
     * since the number of leaves is not transfered between searches in the
     * or-tools solver. Thus, it creates problems when nested searches are
     * used.
     *
     * @return the number of candidates
     */
    int64_t CandidatesNumber() const {
        return candidates_;
    }

private:
    // The validator to pass the solution to
    Validator &validator_;

    // The vector of vars (managed outside)
    const IntVarVector vars_;

    // Candidates encountered
    int64_t candidates_ = 0;
};

/**
 * The type for a UDF function creator. It produces an or-tools IntExpr
 * representing the function. Takes as parameters: the solver to use
 * with, the adapter for accessing data, a vector of variables to work
 * with and integer parameters.
 */
typedef IntExpr *(* UDFFunctionCreator)(Solver *, AdapterPtr,
        const std::vector<IntVar *> &, const std::vector<int64> &);

/**
 * This class allows the search process to access data both via sampling
 * and real data. This class also provides the tools necessary to make
 * this access as efficient as possible. It provides a number of register
 * API functions via which the user can register search primitives. The rest
 * is handled by the Searchlight itself.
 */
class Searchlight : private boost::noncopyable {
public:
    /**
     * Status of Searchlight
     */
    enum class Status {
        VOID,      //!< VOID Non-initialized state
        PREPARED,  //!< PREPARED Main solver prepared
        SEARCHING, //!< SEARCHING Main solver is searching (suring Solve())
        FIN_SEARCH,//!< FIN_SEARCH Main solver finished completely
        FIN_VALID, //!< FIN_VALID Validator finished local work
        COMMITTED, //!< COMMITTED SL completed successfully
        TERMINATED //!< TERMINATED Abnormal termination on error
    };

    /**
     * Maps UDF names to UDF creators.
     */
    typedef std::map<std::string, UDFFunctionCreator> UDFMapper;

    /**
     * Creates the main searchlight class. An instance of this class
     * corresponds to a single search process.
     *
     * @param task Searchlight task performing the query
     * @param name the name of the search
     * @param dll_handler handler for loading/unloading DLLs
     */
    Searchlight(SearchlightTask &task,
            const std::string &name,
            DLLHandler &dll_handler) :
        solver_(name),
        db_(nullptr),
        vars_leaf_(&solver_),
        validator_(nullptr),
        validator_thread_(nullptr),
        array_desc_(nullptr),
        dl_udf_handle_(dll_handler.LoadDLL("searchlight_udfs")),
        sl_task_(task),
        status_(Status::VOID),
        solver_balancing_enabled_(true) {}

    /**
     * The destructor.
     */
    ~Searchlight();

    /**
     * Returns the solver to use for the search.
     *
     * @return the solver for the search
     */
    Solver &GetSolver() {
        return solver_;
    }

    /**
     * Returns a constant reference to the solver.
     *
     * @return constant solver reference
     */
    const Solver &GetSolver() const {
        return solver_;
    }

    /**
     * Registers a data array and the corresponding sample for the search.
     *
     * We do not check the correspondence of the array and the sample, since
     * there are probably no means to do that.
     *
     * @param data the data array
     * @param samples samples available for the data array
     */
    void RegisterArray(ArrayPtr &data, const ArrayPtrVector &samples) {
        array_desc_ = new SearchArrayDesc(data, samples);
    }

    /**
     * Registers an attribute for the search. All further adapter data
     * accesses must go through the returned id.
     *
     * @param name the attribute's name
     * @return the access id for the attribute
     */
    AttributeID RegisterAttribute(const std::string &name) {
        if (!array_desc_) {
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
                    << "No array registered with SL to register an attribute";
        }
        return array_desc_->RegisterAttribute(name,
                config_.get("searchlight.load_aux_samples", 0));
    }

    /**
     * The main solve method that starts the search. It is assumed that the
     * Searchlight was prepared by calling Prepare() function.
     */
    void Solve();

    /**
     * Prepares Searchlight for execution. This method does not start the
     * search. The search itself is started by the engine (via the operator).
     *
     * This method takes "primary" and "secondary" decision variables.
     * "Primary" variables basically define the location of the object of
     * search (e.g., the left-most corner of a rectangle). "Secondary"
     * variables convey additional information about the object, like perimeter,
     * length and so on. Essentially, primary variables are used to determine
     * search distribution across multiple instances.
     *
     * @param primary_vars "primary" decision variables
     * @param secondary_vars "secondary" decision variables
     * @param db the decision builder (search heuristic)
     * @param vars the decision variables
     * @param monitors monitors, if required (can be empty)
     * @return true, if the search found something; false otherwise
     */
    void Prepare(const IntVarVector &primary_vars,
            const IntVarVector &secondary_vars,
            DecisionBuilder *db, const std::vector<SearchMonitor *> &monitors);

    /**
     * Returns the creator for the requested UDF.
     *
     * @param name the NAME of the UDF
     * @return pointer to the creator function
     */
    UDFFunctionCreator GetUDFFunctionCreator(const std::string &name) {
        // first, look in the map
        std::string tag_name = "UDF_" + name;
        UDFFunctionCreator udf = GetRegisteredUDF(tag_name);
        if (udf) {
            return udf;
        }

        // else, look in the library
        std::string func_name = "Create" + tag_name;
        udf = (UDFFunctionCreator)dlsym(dl_udf_handle_,
                func_name.c_str());
        // We should check via dlerror, but NULL checking is fine
        if (!udf) {
            std::ostringstream err_msg;
            err_msg << "Cannot find a SL UDF function, name=" << func_name;
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
                    << err_msg.str();
        }
        udf_map_[tag_name] = udf;
        return udf;
    }

    /**
     * Returns the creator for the registered UDF.
     *
     * @param tag_name the tag name for the UDF
     * @return the creator if found; NULL otherwise
     */
    UDFFunctionCreator GetRegisteredUDF(const std::string &tag_name) const {
        UDFMapper::const_iterator udf_it = udf_map_.find(tag_name);
        if (udf_it != udf_map_.end()) {
            return udf_it->second;
        } else {
            return NULL;
        }
    }

    /**
     * Return all UDFs requested from this SL.
     *
     * @return a set of UDF names requested from this SL.
     */
    StringSet GetAllUsedUDFs() const {
        StringSet res;
        for (UDFMapper::const_iterator cit = udf_map_.begin();
                cit != udf_map_.end(); cit++) {
            res.insert(cit->first);
        }
        return res;
    }

    /**
     * Creates an adapter to access the search array.
     *
     * @param name the adapater's name
     * @return access adapter
     */
    AdapterPtr CreateAdapter(const std::string &name) const {
        return std::make_shared<Adapter>(*array_desc_, name);
    }

    /**
     * Creates a new defaul SL search heuristic. The caller is responsible
     * for destruction.
     *
     * @param primary_vars primary decision variables
     * @param secondary_vars secondary variables
     *
     * @return SL search heuristic
     */
    DecisionBuilder *CreateDefaultHeuristic(const IntVarVector &primary_vars,
            const IntVarVector &secondary_vars);

    /**
     * Returns user monitors attached to the main solver during the search.
     * Since this function returns a reference to the internal list, it is safe
     * to call before the Solve(). The list will be populated there. This also
     * means that checking the list before the search starts is meaningless,
     * since the list will be empty or incomplete.
     *
     * @return solver monitors defined by the user
     */
    const std::vector<SearchMonitor *> &GetUserMonitors() const {
        return search_monitors_.user_monitors_;
    }

    /**
     * Returns monitors established by Searchlight during the search.
     * Since this function returns a reference to the internal list, it is safe
     * to call before the Solve(). The list will be populated there. This also
     * means that checking the list before the search starts is meaningless,
     * since the list will be empty or incomplete.
     *
     * @return solver monitors defined by Searchlight
     */
    const std::vector<SearchMonitor *> &GetAuxMonitors() const {
        return search_monitors_.aux_monitors_;
    }

    /**
     * Returns the monitor that submits candidates to the validator. While it
     * is safe to call before Solve(), the function will return nullptr if
     * no monitor was established yet.
     *
     * @return the validator monitor or nullptr, if there is none
     */
    SearchMonitor *GetValidatorMonitor() const {
        return search_monitors_.validator_monitor_;
    }

    /**
     * Causes the searchlight search to terminate. Note, it does not terminate
     * immediately, but within a reasonable amount of time.
     */
    void Terminate() {
        status_ = Status::TERMINATED;
    }

    /**
     * Checks if the searchlight is terminating.
     *
     * @return true, if we are terminating; false otherwise
     */
    bool CheckTerminate() const {
        return status_ == Status::TERMINATED;
    }

    /**
     * Returns the property tree containing configuration options.
     *
     * @return property tree with the config
     */
    const SearchlightConfig &GetConfig() const {
        return config_;
    }

    /**
     * Reads config from the specified file. Only JSON is supported for now.
     *
     * @param file_name path to the file containing config
     */
    void ReadConfig(const std::string &file_name);

    /**
     * Returns the validator for the current search, if any.
     *
     * @return current validator or nullptr, if no exists
     */
    Validator& GetValidator() {
        return *validator_;
    }

    /**
     * Returns current Searchlight status.
     *
     * @return current Searchlight status
     */
    Status GetStatus() const {
        return status_;
    }

    /**
     * Reports locally finished validator.
     */
    void ReportFinValidator();

    /**
     * Handles the end-of-search request from the coordinator.
     *
     * This request basically means that all sorver processes finished. So,
     * we just change status and wake up validator if needed.
     */
    void HandleEndOfSearch();

    /**
     * Handles the commit request from the coordinator.
     *
     * This request basically means that everything is finished. We have to
     * change the status. Strictly speaking, we don't need to wake up the
     * validator -- it will be done at dtor. But we do it to reclaim
     * resources early.
     */
    void HandleCommit();

    /**
     * Converts the specified solution to a string representation.
     *
     * @param vals variable values
     * @return string representation of the solution
     */
    std::string SolutionToString(const std::vector<int64_t> &vals) const;

    /**
     * Checks if any instances are available for help.
     *
     * @return true, if help available; false, otherwise
     */
    bool HelpAvailable() const {
        return !helpers_.empty();
    }

    /**
     * Handles a new helper for this solver.
     *
     * This handler also might reject help is the balancing is disabled (hard)
     * or if the solver is not prepared or searching (soft). The latter might
     * happen because of asynchronous messages, even when the solver has
     * already became idle.
     *
     * @param id helper instance id
     */
    void HandleHelper(InstanceID id);

    /**
     * Dispatches work to an available helper.
     *
     * It is assumed that a helper is available, which can be checked by
     * the HelpAvailable() function.
     *
     * @param work assignments to off-load
     */
    void DispatchWork(const LiteAssignmentVector &work);

    /**
     * Prepares this solver as a helper.
     *
     * This function loads remote assignments into the solver.
     *
     * @param load remote load
     */
    void PrepareHelper(LiteAssignmentVector &load);

    /**
     * Return primary variables for the search.
     *
     * Primary variables are the variables that are used to divide the
     * search between instances, perform load balancing and make decisions
     * during the search.
     *
     * @return primary variables
     */
    const IntVarVector &GetPrimaryVars() const {
        return primary_vars_;
    }

private:
    // Signals validator to end, waits and then destroys it
    void EndAndDestroyValidator();

    // Reject/release helpers
    void RejectHelpers(bool hard);

    // Monitors participating in the search
    struct SearchMonitors {
        // Validator monitor for the search
        ValidatorMonitor *validator_monitor_ = nullptr;

        // User monitors
        std::vector<SearchMonitor *> user_monitors_;

        // Auxiliary monitors established by SL
        std::vector<SearchMonitor *> aux_monitors_;

        void Disband() {
            delete validator_monitor_;
        }

        // Returns a vector of all monitors that we install for search
        std::vector<SearchMonitor *> GetSearchMonitors() const {
            std::vector<SearchMonitor *> mons{user_monitors_};
            mons.insert(mons.end(), aux_monitors_.begin(), aux_monitors_.end());
            if (validator_monitor_) {
                mons.push_back(validator_monitor_);
            }
            return mons;
        }
    };

    // The solver
    Solver solver_;

    // Search heuristic
    DecisionBuilder *db_;

    // "Primary" decision variables -- defining the object
    IntVarVector primary_vars_;

    // "Secondary" decision variables -- conveying additional info
    IntVarVector secondary_vars_;

    // Contains all vars: for prototyping and sending to validator
    Assignment vars_leaf_;

    // Monitors defined for the search
    SearchMonitors search_monitors_;

    // Validator for the search and its thread
    Validator *validator_;
    std::thread *validator_thread_;

    // The array descriptor
    SearchArrayDesc *array_desc_;

    // The udfs library
    void *dl_udf_handle_;

    // Maps requested UDF names to corresponding creators
    UDFMapper udf_map_;

    // Total time spent on solving
    std::chrono::microseconds total_solve_time_;

    // Searchlight task
    SearchlightTask &sl_task_;

    // Contains various configuration options
    SearchlightConfig config_;

    // Searchlight status
    std::atomic<Status> status_;

    // Helpers currently dispatched for this solver
    std::list<InstanceID> helpers_;

    // Work given to us by another instance
    LiteAssignmentVector helper_load_;

    // Do we accept help?
    bool solver_balancing_enabled_;

    // Total number of candidates met
    uint64_t candidates_ = 0;

    // For concurrency control
    std::mutex mtx_;
};

/**
 * Creates a cumulative time limit, applied to all nested searched combined.
 *
 * Users of the SL heuristic should use this time limit, since SL heuristic
 * might run a lot of nested searches. Unfortunately, or-tools doesn't have
 * an interface function that allows to set the ccumulativity. Thus, this
 * function.
 *
 * Users don't need to take care of freeing the memory. The solver will do it.
 *
 * @param s the solver to use the timer for
 * @param time_ms the cumulative time limit in ms
 * @return the time limit monitor
 */
SearchMonitor *MakeCumulativeTimeLimit(Solver &s, int64 time_ms);

} /* namespace searchlight */
#endif /* SEARCHLIGHT_SEARCHLIGHT_H_ */
