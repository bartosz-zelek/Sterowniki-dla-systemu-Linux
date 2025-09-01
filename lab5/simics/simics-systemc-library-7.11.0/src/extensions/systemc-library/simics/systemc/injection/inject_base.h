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

#ifndef SIMICS_SYSTEMC_INJECTION_INJECT_BASE_H
#define SIMICS_SYSTEMC_INJECTION_INJECT_BASE_H



#include <simics/systemc/injection/inject_interface.h>
#include <simics/systemc/registry.h>

#include <tlm>

#include <string>

namespace simics {
namespace systemc {
namespace injection {

#define INJECT_SET_VALUE(setter, type, extension)               \
    if (key == #setter) {                                       \
        type v = 0;                                             \
                                                                \
        if (!parser->value(&v)) {                               \
            return false;                                       \
        }                                                       \
        this->template get_extension<extension>(gp)->setter(v); \
        return true;                                            \
    }

/** \internal */
template <typename TPAYLOAD>
class InjectBase : public Registrant<InjectInterface<TPAYLOAD> > {
  public:
    InjectBase() : gp_(NULL) {}
    virtual ~InjectBase() {}
    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) = 0;
    virtual void attrToValue(AttrDictParser *parser, TPAYLOAD *gp) {
        gp_ = gp;
        parser->parse(this);
    }
    virtual void released(TPAYLOAD *gp) {}
    virtual bool parse(AttrDictParser *parser, const std::string &key,
                       attr_value_t *attr) {
        return setValue(parser, key, attr, gp_);
    }
    template<typename T>
    T *get_extension(TPAYLOAD *gp) {
        T *t = gp->template get_extension<T>();
        if (t)
            return t;

        // The created extension is deleted in the InjectRegistry when the
        // GP is deleted.
        gp->set_extension(new T);
        return gp->template get_extension<T>();
    }

  private:
    TPAYLOAD *gp_;
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
