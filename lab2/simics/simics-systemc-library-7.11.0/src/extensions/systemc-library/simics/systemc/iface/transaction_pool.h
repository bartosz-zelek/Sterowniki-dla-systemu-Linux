// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_TRANSACTION_POOL_H
#define SIMICS_SYSTEMC_IFACE_TRANSACTION_POOL_H

#include <simics/systemc/iface/transaction.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/utility.h>
#include <tlm>

#include <deque>
#include <set>

namespace simics {
namespace systemc {
namespace iface {

/** A memory manager that implements the tlm::tlm_mm_interface providing a pool
 *  of transaction objects.
 */
class TransactionPool : public tlm::tlm_mm_interface {
    typedef tlm::tlm_generic_payload gp_t;

  public:
    TransactionPool()
        : active_cnt_(0) {
    }

    ~TransactionPool() {
        // Once the TransactionPool object is destroyed, the pool_ member is
        // invalid. The deleter avoids pushing the payload back into an invalid
        // member and deletes the payload. The delete of the payload deletes
        // the TransactionExtension as well.
        static struct : public tlm::tlm_mm_interface {
             virtual void free(tlm::tlm_generic_payload *gp) {
                 delete gp;
             }
         } deleter;

        for (std::set<gp_t *>::iterator it = all_payloads_.begin();
            it != all_payloads_.end(); ++it) {
            if ((*it)->get_ref_count() == 0) {
                delete *it;
            } else {
                (*it)->set_mm(&deleter);
            }
        }
    }

    Transaction acquire() {
        gp_t *gp = NULL;
        if (pool_.empty()) {
            // mm object is explicit passed when create gp_t
            // there is no need to call set_mm
            gp = new gp_t(this);
            // Set sticky extension. Deleted in ~tlm_generic_payload()
            gp->set_extension(new iface::TransactionExtension);
            all_payloads_.insert(gp);
        } else {
            gp = pool_.front();
            pool_.pop_front();
        }

        ++active_cnt_;
        return Transaction(gp);
    }

    // free method is invoked when the reference count of the GP
    // is reduced to zero
    void free(gp_t *transaction_ptr) {
        // For sticky extensions, they will be reused without cleared
        transaction_ptr->reset();
        simics::systemc::utility::reset_payload(transaction_ptr);
        pool_.push_back(transaction_ptr);
        --active_cnt_;
        assert(active_cnt_ >= 0 && "Active number of GP becomes negative");
    }

    unsigned PoolSize() const {
        return pool_.size();
    }

    int active_cnt() const {
        return active_cnt_;
    }

  private:
    // queue holding payloads currently not allocated
    std::deque<gp_t *> pool_;
    std::set<gp_t *> all_payloads_;
    // Active number of GP in use right now
    int active_cnt_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
