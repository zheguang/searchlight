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
 * @file searchlight_messenger.h
 *
 * This file describes the Searchlight messenger. This class is responsible
 * with communicating with other instances to request chunks, send/receive
 * control messages, etc.
 *
 * The class uses internal SciDb NetworkManages services, including
 * networking and message formats. It registers new UDTs for messages, so
 * extra-care should be taken to avoid clashes with other message types,
 * e.g., MPI ones.
 *
 * @author Alexander Kalinin
 */

#ifndef SEARCHLIGHT_SEARCHLIGHT_MESSENGER_H_
#define SEARCHLIGHT_SEARCHLIGHT_MESSENGER_H_

#include "scidb_inc.h"

#include <mutex>
#include <condition_variable>
#include <unordered_map>

namespace searchlight {

/**
 * This class represents SearchlightMessenger. It allows communticating with
 * other instances running Searchlight to exchange chunks, control
 * messages, etc. This class is a singleton instantiated by the first user.
 * Then, each user should call RegisterQuery() to initialize it (only the first
 * call for the given query will come through).
 * The Messenger will deactivate itself automatically via the query finalizer.
 *
 * Messenger allows exchanging chunks of the arrays. For this, each array
 * must be registered at the Messenger. Otherwise, a request will cause an
 * exception.
 *
 * We will reuse Fetch and Chunk protobuf messages, already implemented at
 * SciDb. Unfortunately, we need to define our own handler and creator
 * callbacks, since mtFetch/mtChunk messages are considered system and, thus,
 * handled by SciDb internally. We re-use protobuf messages as typed data
 * carriers only.
 */
class SearchlightMessenger : public scidb::Singleton<SearchlightMessenger> {
public:
    /**
     *  Constructs the messenger. For now it just registers message handlers.
     */
    SearchlightMessenger() {
        RegisterHandlers(true);
        /*
         * We do not remove message handlers on destruction. First of all,
         * SciDb does not have API for that -- handlers remain until
         * termination. Secondly, we just throw an exception if a request came
         * out of context (query). That will happen during query validation.
         *
         * An alternative would be to establish empty handlers, so SciDb
         * would drop messages by itself. However, we won't detect errors
         * that way.
         */
    }

    /**
     * Initializes the messenger for a new query. Can be called several times
     * within the same query -- only the first call counts. The messenger
     * will deactivate itself automatically, when the query is finished.
     *
     * @param query current query
     */
    void RegisterQuery(const boost::shared_ptr<Query> &query);

    /**
     * Registers array for chunk exchange. Instances can exchange chunks only
     * for arrays registered at the messenger. All arrays are unregistered
     * when the current query finishes.
     *
     * @param query caller's query
     * @param array array to register
     */
    void RegisterArray(const boost::shared_ptr<Query> &query,
            const ArrayPtr &array);

    /**
     * Requests a chunk from the the specified array. The caller has to
     * explicitly specify what instance to request the chunk from. If the
     * caller does not specify a pointer for the chunk to be written, only
     * the existence of the chunk at the specified position is confirmed.
     * If the chunk is specified, it is supposed to be pinned in memory.
     * This function does not un-pin it; that is the caller's responsibility.
     *
     * @param query caller's query
     * @param inst  instance to requests the chunk from (logical ID)
     * @param array_name name of the array
     * @param pos   position of the hunk
     * @param attr  chunk attribute
     * @param chunk pointer to the destination (nullptr means confirm-only)
     * @return true, if the chunk exists; false, no chunk found
     */
    bool RequestChunk(const boost::shared_ptr<Query> &query,
            InstanceID inst, const std::string &array_name,
            const Coordinates &pos, AttributeID attr, Chunk *chunk);

private:

    struct ChunkRequestData {
        ArrayPtr array_;  // array
        Chunk *chunk_;    // chunk data (nullptr -- no data required)
        bool empty_;      // is the chunk empty?
    };

    /*
     * Message slot. Used by a requester to wait for the result and to return
     * data. Each caller gets a slot.
     */
    struct MessageSlot {
        bool used_ = true; // is slot being used? for reusing the slot

        // sync waiting
        std::mutex mtx_;
        std::condition_variable cond_;

        // response data (type depends on the caller, e.g., ChunkRequestData)
        void *data_ = nullptr;
        bool data_ready_ = false; // is data ready?
    };
    std::deque<MessageSlot> slots_;

    /*
     * Every query served by the messenger has its own context. A query must
     * be explicitly registered with the messenger via the register() function.
     * Subsequent RegisterQuery() calls for the same query will be ignored.
     * When the query finished its context is de-allocated via a finalizer.
     */
    struct QueryContext {
        // Represents an array with cached iterators
        struct ServedArray {
            const ArrayPtr array_;
            std::unordered_map<AttributeID,
                boost::shared_ptr<ConstArrayIterator>> iters_;

            ServedArray(const ArrayPtr &array) : array_(array) {}
        };
        typedef std::shared_ptr<ServedArray> ServedArrayPtr;

        // Reference to the query
        const boost::weak_ptr<Query> query_;

        // Arrays registered for chunk exchange
        std::unordered_map<std::string, ServedArrayPtr> reg_arrays_;

        QueryContext(const boost::shared_ptr<Query> &query) :
            query_(query) {}
    };
    typedef std::shared_ptr<QueryContext> QueryContextPtr;
    std::unordered_map<QueryID, QueryContextPtr> served_queries_;

    // To guard the structure
    mutable std::mutex mtx_;

    // Chunk request: record creator and handler
    MessagePtr CreateSLChunkRequest(MessageID id);
    void HandleSLChunkRequest(
            const boost::shared_ptr<MessageDescription> &msg_desc);

    // Chunk response: record creator and handler
    MessagePtr CreateSLChunk(MessageID id);
    void HandleSLChunk(
            const boost::shared_ptr<MessageDescription> &msg_desc);

    // Deactivates the messenger from the query
    void deactivate(const boost::shared_ptr<Query> &query);

    // Register message handlers. init = true means we are initializing.
    void RegisterHandlers(bool init);

    // Returns a new message slot (or re-uses an old one)
    uint32_t GetMessageSlot();

    // Returns a message slot
    void ReturnMessageSlot(size_t slot);

    // Returns pointer to the registered array with name
    QueryContext::ServedArrayPtr GetRegisteredArray(
            const boost::shared_ptr<Query> &query,
            const std::string &name) const;

    // Returns valid pointer to the registered query
    boost::shared_ptr<Query> GetRegisteredQuery(QueryID query_id) const;

    // Returns query context for the given id
    QueryContextPtr GetQueryContext(QueryID query_id) const;
};
}
#endif /* SEARCHLIGHT_SEARCHLIGHT_MESSENGER_H_ */
