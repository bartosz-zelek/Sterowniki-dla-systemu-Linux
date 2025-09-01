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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_ETHERNET_COMMON_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_ETHERNET_COMMON_H



#include <simics/systemc/iface/ethernet_common_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>
#include <simics/types/frags.h>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectEthernetCommon : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("ethernet_common.")

    bool extractFragsFrag(AttrDictParser *parser, const attr_value_t *attr,
                          simics::types::frags_frag_t *frag) {
        if (!SIM_attr_is_data(*attr)) {
            parser->reportError("fragslist must be data");
            return false;
        }
        frag->start = SIM_attr_data(*attr);
        frag->len = SIM_attr_data_size(*attr);
        return true;
    }
    bool extractFrags(AttrDictParser *parser, const attr_value_t *attr,
                      simics::types::frags_t *frags) {
        AttrDictParser p = parser->init(attr);

        if (!p.lookUp("len", &frags->len))
            return false;

        std::vector<attr_value_t> fraglist;
        if (!p.lookUp("fraglist", &fraglist))
            return false;

        if (fraglist.size() > MAX_FRAGS_FRAGS) {
            parser->reportError("fraglist must not exceed %i elements",
                                MAX_FRAGS_FRAGS);
            return false;
        }
        frags->nfrags = fraglist.size();

        std::vector<attr_value_t>::iterator i;
        int cnt = 0;
        for (i = fraglist.begin(); i != fraglist.end(); ++i) {
            simics::types::frags_frag_t *frag = &frags->fraglist[cnt++];
            if (!extractFragsFrag(parser, &(*i) , frag))
                return false;
        }

        return true;
    }
    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "frame") {
            AttrDictParser p = parser->init(attr);

            attr_value_t frame_attr;
            if (!p.lookUp("frame", &frame_attr))
                return false;

            frames_.insert(std::make_pair(gp, new simics::types::frags_t));
            simics::types::frags_t *frame = frames_[gp];
            if (!extractFrags(parser, &frame_attr, frame))
                return false;

            int32_t crc_ok = 0;
            if (!p.lookUp("crc_ok", &crc_ok))
                return false;

            ExtensionSetter<TPAYLOAD, iface::EthernetCommonExtension>(gp)
                ->frame(frame, crc_ok);
            return true;
        }

        return false;
    }
    virtual void released(TPAYLOAD *gp) {
        if (frames_.find(gp) != frames_.end()) {
            delete frames_[gp];
            frames_.erase(gp);
        }
    }

  private:
    std::map<TPAYLOAD *, simics::types::frags_t *> frames_;
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
