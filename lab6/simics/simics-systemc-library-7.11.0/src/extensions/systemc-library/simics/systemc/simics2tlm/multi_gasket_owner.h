// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_MULTI_GASKET_OWNER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_MULTI_GASKET_OWNER_H



#include <simics/systemc/simics2tlm/gasket_interface.h>
#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/null_gasket.h>

#include <map>
#include <set>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Container class for multiple GasketOwners, each given a unique ID. This is
 * useful for protocols where the interface implementation can de-mux a single
 * Simics interface into multiple TLM target sockets, like the IoMemory for
 * example. There must always be at least one valid gasket, which is guaranteed
 * by the GasketOwner base class. See findGasket() for details.
 */
class MultiGasketOwner : public GasketOwner {
  public:
    void addGasket(int id, GasketInterface::Ptr gasketInterface) {
        FATAL_ERROR_IF(hasId(id),
                       "Cannot bind the same ID (%d) to multiple gaskets", id);
        // TODO(ah): when we move to C++11 we can store the GasketOwner
        // instance in the map instead of a pointer, in order to simplify this
        // class a bit. In pre-11 we cannot do this as GasketOwner is
        // non-copyable and there are no move-constructors in pre-11. As an
        // alternative, we could use boost::shared_ptr to keep track of
        // GasketOwner's resource pointer and make it copyable.
        GasketOwner *gasket_owner = new GasketOwner;
        *static_cast<ClassType *>(gasket_owner) =
            *static_cast<ClassType *>(this);
        // coverity[copy_instead_of_move]
        gasket_owner->set_gasket(gasketInterface);
        gasket_map_[id] = gasket_owner;
        // Need to delete NullGasket if it exists
        if (dynamic_cast<NullGasket*>(gasket_.get()))
            gasket_ = NULL;
    }

    /**
     * Returns the gasket matching the ID given. If no such ID could be found,
     * the gasket with the lowest ID is returned instead. If no gasket was
     * added to this container with an ID, the gasket assigned to the base
     * class (by GasketOwner::set_gasket()) is returned. The base class
     * guarantees that such a gasket must exist.
     */
    GasketInterface::Ptr findGasket(int id) const {
        GasketOwnerMap::const_iterator it = gasket_map_.find(id);
        if (it != gasket_map_.end()) {
            return it->second->gasket();
        } else if (!empty()) {
            return gasket_map_.begin()->second->gasket();
        } else {
            return gasket();
        }
    }
    std::set<int> keys() const {
        std::set<int> k;
        for (auto i = gasket_map_.begin(); i != gasket_map_.end(); ++i)
            k.insert(i->first);

        return k;
    }
    virtual ~MultiGasketOwner() {
        for (GasketOwnerMap::iterator it = gasket_map_.begin();
             it != gasket_map_.end(); ++it) {
            delete it->second;
        }
    }

  protected:
    bool hasId(int id) const {
        return gasket_map_.count(id);
    }
    int empty() const {
        return gasket_map_.empty();
    }

  private:
    typedef std::map<int, GasketOwner*> GasketOwnerMap;
    GasketOwnerMap gasket_map_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
