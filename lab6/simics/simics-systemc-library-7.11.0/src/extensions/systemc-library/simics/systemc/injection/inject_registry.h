// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_INJECTION_INJECT_REGISTRY_H
#define SIMICS_SYSTEMC_INJECTION_INJECT_REGISTRY_H



#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/inject_interface.h>
#include <simics/systemc/registry.h>

#include <tlm>

#include <map>
#include <string>
#include <utility>

namespace simics {
namespace systemc {
namespace injection {

/** \internal */
template <typename TPAYLOAD>
class InjectRegistry : public tlm::tlm_mm_interface {
  public:
    TPAYLOAD *attrToPayload(conf_object_t *obj, attr_value_t *attr) {
        gp_ = new TPAYLOAD(this);
        gp_->acquire();
        attr_value_t attrCopy = SIM_attr_copy(*attr);
        gpAttrCopies_.insert(std::make_pair(gp_, attrCopy));
        AttrDictParser parser(obj, &attrCopy);
        parser_ = &parser;
        // Reverse iterate over all objects implementing
        // InjectInterface<TPAYLOAD> and invoke
        // bool operator()(InjectInterface<TPAYLOAD>* iface)
        Registry<InjectInterface<TPAYLOAD> >::instance()->reverseIterate(this);
        if (parser.reportInvalidAttrs())
            return gp_;

        gp_->release();
        return NULL;
    }
    bool operator()(InjectInterface<TPAYLOAD>* iface) {
        iface->attrToValue(parser_, gp_);
        return false;
    }

  private:
    struct ReleasedInvoker {
        bool operator()(InjectInterface<TPAYLOAD>* iface) {
            iface->released(gp_);
            return false;
        }
        TPAYLOAD *gp_;
    };
    virtual void free(tlm::tlm_generic_payload *gp) {
        released_.gp_ = gp;
        Registry<InjectInterface<TPAYLOAD> >::instance()->reverseIterate(
            &released_);

        typename std::map<TPAYLOAD *, attr_value_t>::iterator i;
        i = gpAttrCopies_.find(gp);
        if (i != gpAttrCopies_.end()) {
            SIM_attr_free(&i->second);
            gpAttrCopies_.erase(i);
        }

        delete gp;
    }

    TPAYLOAD *gp_;
    ReleasedInvoker released_;
    AttrDictParser *parser_;
    std::map<TPAYLOAD *, attr_value_t> gpAttrCopies_;
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
