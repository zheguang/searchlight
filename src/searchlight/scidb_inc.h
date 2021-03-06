/*
 * Copyright 2014, Brown University, Providence, RI.
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
 * @file scidb_inc.h
 * This is common file for other sources using SciDb. It contains
 * common definitions and includes from SciDb.
 *
 * @author Alexander Kalinin
 */

#ifndef SEARCHLIGHT_SCIDB_INC_H_
#define SEARCHLIGHT_SCIDB_INC_H_

/*
 * A workaround to avoid compiler errors when SciDb returns shared_ptrs
 * from functions with bool return types, which requires explicit conversion.
 * This works until we move to C++11, for which Boost dumps the safe bool idiom
 * and goes with the explicit bool conversion operator.
 */
#define BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS

/*
 * A workaround to avoid std/boost::shared_ptr in SciDb, since we are
 * working with -std=c++0x
 */
#include <boost/shared_ptr.hpp>
namespace scidb {
    using boost::shared_ptr;
}

#include <array/Metadata.h>
#include <array/Array.h>
#include <array/StreamArray.h>
#include <array/DelegateArray.h>
#include <system/Config.h>

#include <util/NetworkMessage.h>
#include <network/BaseConnection.h>
#include <network/NetworkManager.h>
#include <util/Network.h>
#include <network/proto/scidb_msg.pb.h>
#include <network/MessageUtils.h>

#include <unordered_set>
#include <unordered_map>

namespace searchlight {

// System structures
using scidb::SystemCatalog;
using scidb::Query;
using scidb::QueryID;
using scidb::Config;

// Array strcuctures
using scidb::Attributes;
using scidb::AttributeID;
using scidb::AttributeDesc;
using scidb::ArrayDesc;
using scidb::ArrayDistribution;
using scidb::Coordinate;
using scidb::Coordinates;
using scidb::DimensionDesc;
using scidb::Address;

// Arrays
using scidb::Array;
using scidb::MemArray;
using scidb::StreamArray;
using scidb::DelegateArray;

// Chunks
using scidb::ConstChunk;
using scidb::Chunk;
using scidb::MemChunk;
using scidb::SharedBuffer;
using scidb::CompressedBuffer;

// Iterators
using scidb::ConstArrayIterator;
using scidb::ArrayIterator;
using scidb::DelegateArrayIterator;
using scidb::ConstItemIterator;
using scidb::ConstChunkIterator;
using scidb::ChunkIterator;

// Aggregate stuff
using scidb::Aggregate;
using scidb::AggregatePtr;
using scidb::AggregateLibrary;

// Types
using scidb::TypeLibrary;
using scidb::Type;

// Chunk values
using scidb::RLEPayload;
using scidb::ConstRLEEmptyBitmap;
using scidb::Value;

// Exceptions
using scidb::SCIDB_SE_OPERATOR;
using scidb::SCIDB_LE_ILLEGAL_OPERATION;
using scidb::SCIDB_SE_EXECUTION;
using scidb::SCIDB_LE_NO_CURRENT_CHUNK;

// Network stuff
using scidb::MessageID;
using scidb::MessagePtr;
using scidb::MessageDescription;
using scidb::InstanceID;
using scidb::NetworkManager;

/**
 *  Pointer for an Array (very common in SciDb).
 */
typedef boost::shared_ptr<Array> ArrayPtr;

/**
 * Map for attributes: name -> ID.
 */
typedef std::unordered_map<std::string, AttributeID> AttributeMap;

/**
 * Vector of SciDb array pointers.
 */
typedef std::vector<ArrayPtr> ArrayPtrVector;

/**
 * A vector of Values
 */
typedef std::vector<Value> ValueVector;

/**
 * Value with the type
 */
typedef std::pair<Value, Type> TypedValue;

/**
 * A vector of typed values
 */
typedef std::vector<TypedValue> TypedValueVector;

/**
 * ConstItemIterator pointer
 */
typedef boost::shared_ptr<ConstItemIterator> ConstItemIteratorPtr;

/**
 * ConstArrayIterator pointer
 */
typedef boost::shared_ptr<ConstArrayIterator> ConstArrayIteratorPtr;

/**
 * Simple hash for scidb::Address to use with unordered containers.
 */
struct AddressHash {
    size_t operator()(const Address &addr) const {
        size_t h = 17;
        h = 31 * h + addr.attId;
        for (auto x: addr.coords) {
            h = 31 * h + static_cast<size_t>(x);
        }
        return h;
    }
};

/**
 * Simple hash for scidb::Coordinates to use with unordered containers.
 */
struct CoordinatesHash {
    size_t operator()(const Coordinates &coords) const {
        size_t h = 17;
        for (auto x: coords) {
            h = 31 * h + static_cast<size_t>(x);
        }
        return h;
    }
};

/**
 * Set of coordinates.
 */
typedef std::unordered_set<Coordinates, CoordinatesHash> CoordinateSet;

/**
 * One-dimensional coordinates.
 */
typedef std::array<Coordinate, 1> Coordinates1D;

/**
 * Convenience struct for storing a region: a pair of coordinate vectors.
 */
struct Region {
    Coordinates low_, high_;

    /**
     * Return the area of the region.
     *
     * @return region area
     */
    size_t Area() const {
        size_t res = 1;
        for (size_t i = 0; i < low_.size(); i++) {
            res *= high_[i] - low_[i] + 1;
        }
        return res;
    }

    /**
     * Return the ratio of this region's area to the other's.
     *
     * @param other other region
     * @return areas ratio
     */
    double AreaRatio(const Region &other) const {
        assert(low_.size() == high_.size());
        assert(low_.size() == other.low_.size());
        assert(other.low_.size() == other.high_.size());

        double res = 1;
        for (size_t i = 0; i < low_.size(); i++) {
            const double len = high_[i] - low_[i] + 1;
            const size_t other_len = other.high_[i] - other.low_[i] + 1;
            res *= len / other_len;
        }
        return res;
    }
};

/**
 * Row-major iterator over the specified region.
 */
class RowMajorIterator {
public:
    RowMajorIterator(const Coordinates &low, const Coordinates &high) :
        low_{low}, high_{high}, current_{low} {
            if (low_.size() != high_.size() || low_.empty()) {
                throw std::invalid_argument(
                    "Coordinates must be of the same size and not empty");
            }
            for (size_t i = 0; i < low_.size(); ++i) {
                if (low_[i] > high_[i]) {
                    throw std::invalid_argument(
                        "Low coordinates must be strictly less than high");
                }
            }
        }

    /**
     * Prefix increment.
     *
     * @return itself
     */
    RowMajorIterator &operator++() {
        assert(valid_);
        size_t i = current_.size() - 1;
        while ((++current_[i]) > high_[i]) {
            current_[i] = low_[i];
            if (i == 0) {
                valid_ = false;
                break;
            }
            --i;
        }
        return *this;
    }

    /**
     * Check if the iterator is at end.
     *
     * @return true, if points to end; false, otherwise
     */
    bool end() const {
        return !valid_;
    }

    /**
     * Reset the iterator to the initial state.
     */
    void Reset() {
        current_ = low_;
        valid_ = true;
    }

    /**
     * Dereference operator.
     *
     * @return current position
     */
    const Coordinates& operator*() const {
        return current_;
    }

private:
    Coordinates low_, high_, current_;
    bool valid_ = true;
};
} /* namespace searchlight */

#undef BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#endif /* SEARCHLIGHT_SCIDB_INC_H_ */
