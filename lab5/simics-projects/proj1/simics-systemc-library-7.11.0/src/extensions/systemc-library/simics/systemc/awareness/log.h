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

#ifndef SIMICS_SYSTEMC_AWARENESS_LOG_H
#define SIMICS_SYSTEMC_AWARENESS_LOG_H

#include <simics/cc-api.h>

#include <ostream>  // NOLINT(readability/streams)
#include <sstream>
#include <string>

namespace simics {
/** Output stream class using the Simics log API. */
// <add id="awareness/log.h">
// <insert-until text="// EOF awareness/log.h"/></add>
template<log_type_t Type = Sim_Log_Info,
         unsigned Level = 1,
         int Groups = 0>
class LogStream : public std::ostream {
  public:
    explicit LogStream(ConfObjectRef log_obj)
// EOF awareness/log.h
        : std::ostream(&str_buf_), str_buf_(log_obj) {}
    ~LogStream() {
        if (tellp() != std::streampos(0)) {
            flush();
        }
    }

  private:
    class StrBuf : public std::stringbuf {
      public:
        explicit StrBuf(ConfObjectRef log_obj)
            : std::stringbuf(ios_base::out),
              log_obj_(log_obj.object()) {}

      private:
        int sync() {
            std::string msg = str();
            // trim trailing newline (and whitespace)
            std::string::size_type pos = msg.find_last_not_of(" \t\n");
            if (pos != msg.length() - 1) {
                if (pos == std::string::npos) {
                    pos = -1;
                }
                msg.erase(pos + 1);
            }

            VT_log_message(log_obj_, Level, Groups, Type, msg.c_str());
            msg.clear();
            str(msg);
            return std::stringbuf::sync();
        }

        conf_object_t *log_obj_;
    };

    StrBuf str_buf_;
};

}  // namespace simics

#endif
