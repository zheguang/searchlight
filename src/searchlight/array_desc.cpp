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
 * @file array_desc.cpp
 * The implementation of the array descriptor.
 *
 * @author Alexander Kalinin
 */

#include "array_desc.h"

#include "searchlight_messenger.h"

namespace searchlight {

// The logger
static log4cxx::LoggerPtr logger(
        log4cxx::Logger::getLogger("searchlight.array_descriptor"));

AttributeID SearchArrayDesc::RegisterAttribute(const std::string &attr_name,
        bool load_aux_samples) {
    std::map<std::string, AttributeID>::const_iterator map_it =
            attr_to_id_.find(attr_name);
    if (map_it != attr_to_id_.end()) {
        return map_it->second;
    }

    const ArrayDesc &desc = array_->getArrayDesc();
    const Attributes &attrs = desc.getAttributes(true);

    AttributeID orig_id;
    if (!SearchArrayDesc::FindAttributeId(attrs, attr_name, orig_id)) {
        std::ostringstream err_msg;
        err_msg << "Cannot find the attribute in the array: array="
                << desc.getName() << ", attr=" << attr_name;
        throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
                << err_msg.str();
    }

    AttributeID search_id = search_orig_ids_.size();
    search_orig_ids_.push_back(orig_id);
    attr_to_id_[attr_name] = search_id;

    // load the sample
    sampler_.LoadSampleForAttribute(attr_name, search_id);
    LOG4CXX_DEBUG(logger, "Registered attribute, name=" << attr_name <<
            ", orig_id=" << orig_id << ", internal_id=" << search_id);
    return search_id;
}

void SearchArrayDesc::GetDynamicChunksDistribution(
        const boost::shared_ptr<Query> &query, const CoordinateSet &chunk_pos,
        std::vector<int> &distr) const {
    assert(query->getInstancesCount() == distr.size());
    const InstanceID local_inst = query->getInstanceID();
    const SearchlightMessenger *messenger = SearchlightMessenger::getInstance();
    const ArrayDesc &array_desc = array_->getArrayDesc();

    // First, retrieve static info (via catalog)
    for (const auto &pos: chunk_pos) {
        InstanceID chunk_inst = scidb::getInstanceForChunk(
                query, pos, array_desc, array_desc.getPartitioningSchema(),
                boost::shared_ptr<scidb::DistributionMapper>(), 0, local_inst);
        ++distr[chunk_inst];
    }

    // Then, retrieve dynamic info
    messenger->GetDistrChunksInfo(query, array_desc.getName(), chunk_pos,
            distr);
}

void SearchArrayDesc::GetStripesChunkDistribution(
        const CoordinateSet &chunk_pos, std::vector<int> &distr) const {

    const DimensionDesc &split_dim = array_->getArrayDesc().getDimensions()[0];
    const Coordinate low = split_dim.getLowBoundary();
    const uint64_t stripe_len =
            ceil(double(split_dim.getCurrLength()) / distr.size());

    for (const auto &pos: chunk_pos) {
        const InstanceID inst = (pos[0] - low) / stripe_len;
        assert(inst < distr.size());
        distr[inst]++;
    }
}

} /* namespace searchlight */
