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
 * @file adapter.h
 * A class that represent the main access point to the search array.
 * Depending on the situation (i.e., the mode of work) it might be
 * beneficial to access either the sample or the data itself. This class
 * does exactly that.
 *
 * @author Alexander Kalinin
 */

#ifndef SEARCHLIGHT_ADAPTER_H_
#define SEARCHLIGHT_ADAPTER_H_

#include "scidb_inc.h"
#include "base.h"

#include <chrono>

namespace searchlight {

class SearchArrayDesc;

/**
 * Approximate value, returned by an estimator.
 *
 * TODO: there might be some benefits in returning the probability of MAY_NULL
 * in the future.
 */
struct IntervalValue {
    /**
     * Describes the state of the value
     */
    enum State {
        NON_NULL,//!< NON_NULL definitely not NULL
        MAY_NULL,//!< MIN_NULL might be NULL (thus, min is invalid)
        NUL      //!< NUL definitely NULL
    };

    /**
     * Minimum possible
     */
    double min_;

    /**
     * Maximum possible
     */
    double max_;

    /**
     * An approximate value.
     */
    double val_;

    /**
     * Value state
     */
    State state_;

    /**
     * Creates a NULL interval value.
     */
    IntervalValue() : min_(0), max_(0), val_(-1), state_(NUL) {}
};

/**
 * Outputs an IntervalValue into an output stream.
 *
 * @param os the stream to output to
 * @param iv the value to output
 * @return the stream used for the output
 */
std::ostream &operator<<(std::ostream &os, const IntervalValue &iv);

/**
 * A vector of interval values.
 */
typedef std::vector<IntervalValue> IntervalValueVector;

/**
 * This class allows users to access search data. For example, fetching
 * an element by the coordinates or an entire interval/region. It also allows
 * to iterate via elements of a region or call a callback on them. Note, that
 * "data" might either correspond to a sample or the real array. The type
 * of access is determined by the adapter's mode. This class is exposed to
 * the library users for all search data access purposes.
 *
 * Note that data access functions return intervals, instead of single
 * values. This is done to make user's lives easier. An exact value is just
 * an interval of length 0. For example, asking for a(i, j) might give you
 * an exact value from the array, an approximate value generated from a sample
 * or an exact interval that contains a(i, j). This depends on the mode
 * of work, which can be selected explicitly, but is chosen automatically
 * by default depending on the situation.
 */
class Adapter {
public:
    /**
     * This is the mode of work of the adapter, which determines what
     * results it returns.
     */
    enum Mode {
        /** The adapter gives the exact results from the real data */
        EXACT = 0,
        /** The adapter gives approximate values generated from a sample */
        APPROX,
        /** The adapter gives a precise interval of possible values */
        INTERVAL,
    };

    /**
     * Creates an adapter for a SciDb array.
     *
     * @param array the SciDb data array
     */
    Adapter(const SearchArrayDesc &array, const std::string &name) :
        array_desc_(array),
        mode_(Mode::INTERVAL),
        name_(name) {}

    /**
     * Destructor.
     */
    ~Adapter();

    /**
     * Sets the mode of operation for the adapter.
     *
     * @param mode the mode required
     */
    void SetAdapterMode(Mode mode) {
        mode_ = mode;
    }

    /**
     * Computes specified aggregates for the specified regions for the
     * specified attribute. Depending on the mode, the result will be
     * approximate with precise boundaries or exact, computed over the real
     * data. For the exact result, the min/max/val of each result are the same.
     * All aggregates must be registered with the sampler and with SciDb.
     * Default ones are registered automatically.
     *
     * @param low the low coordinates of the region
     * @param high the upper coordinates for the region
     * @param attr the access id of the attribute to compute
     * @param aggr_names the names of the aggregates
     * @return results, one per aggregate in the form of intervals; see the
     *  definition of the interval for details
     */
    IntervalValueVector ComputeAggregate(const Coordinates &low,
            const Coordinates &high, AttributeID attr,
            const StringVector &aggr_names) const;

    /**
     * Returns the value at the specified coordinates. The value can be
     * approximate or exact based on this adapter's mode of operation. In
     * case of the exact value the min/max/val of the result is the same.
     *
     * @param point the point's coordinates
     * @param attr the attribute to compute the value for
     * @return the value
     */
    IntervalValue GetElement(const Coordinates &point, AttributeID attr) const;

private:
    // The search array descriptor
    const SearchArrayDesc &array_desc_;

    // Mode of operation
    Mode mode_;

    // The adapter's name
    const std::string name_;

    // Total time spent for requests
    mutable std::chrono::microseconds total_req_time_;
};

/**
 * Adapter shared pointer.
 */
typedef boost::shared_ptr<Adapter> AdapterPtr;

} /* namespace searchlight */
#endif /* SEARCHLIGHT_ADAPTER_H_ */