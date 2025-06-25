// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/connection_end_point.h>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/sc_export_connection.h>
#include <simics/systemc/awareness/sc_port_connection.h>

#include <simics/systemc/sim_context_proxy.h>

#include <sysc/communication/sc_port.h>
#if INTC_EXT
#include <sysc/communication/sc_port_int.h>  // access to internal structs

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {
bool sorter(sc_core::sc_port_base *i, sc_core::sc_port_base *j) {
    return strcmp(i->name(), j->name()) <= 0;
}
}  // namespace
#endif

namespace simics {
namespace systemc {
namespace awareness {

using sc_core::sc_bind_info;
using sc_core::sc_export_base;
using sc_core::sc_interface;
using sc_core::sc_port_base;
using std::string;
using std::vector;

std::map<ScPortConnection::keytype,
         ScPortConnection::valuetype> ScPortConnection::ports_;

ScPortConnection::ScPortConnection()
    : base_(NULL), max_number_of_proxies_(0), simulation_(NULL) {
}

void ScPortConnection::init(sc_core::sc_object *object,
                            iface::SimulationInterface *simulation) {
    simulation_ = simulation;
    base_ = dynamic_cast<sc_port_base *>(object);
    if (!base_)
        return;

#if INTC_EXT
    base_typename_ = sc_core::simContextProxy::get_typename(base_);

    sc_bind_info *info = sc_core::simContextProxy::get_bind_info(base_);
    if (!info)
        return;

    max_number_of_proxies_ = info->max_size();

    vector<sc_core::sc_bind_elem *>::iterator i;
    for (i = info->vec.begin(); i != info->vec.end(); ++i) {
        sc_port_base *parent = (*i)->parent;
        if (parent)
            bind_info_.push_back(parent);

        sc_interface *iface = (*i)->iface;
        if (iface) {
            bind_info_.push_back(iface);
            keytype key = std::make_pair(iface,
                sc_core::simContextProxy::get_typename(base_));
            valuetype &v = ports_[key];
            v.push_back(base_);
            std::sort(v.begin(), v.end(), sorter);
        }
    }
#endif
}

vector<conf_object_t *> ScPortConnection::port_to_proxies() {
    vector<conf_object_t *> proxies;

    for (vector<Item>::iterator i = bind_info_.begin(); i
         != bind_info_.end(); ++i) {
        sc_port_base *parent = i->port_;
        if (parent) {
            ProxyInterface *proxy = Proxy::findProxy(parent);
            proxies.push_back(proxy ? proxy->simics_obj() : NULL);

            continue;
        }

        sc_interface *iface = i->iface_;
        if (iface) {
            vector<sc_export_base *> exports =
                ScExportConnection::findExport(std::make_pair(iface,
                    sc_core::simContextProxy::get_typename(base_)));

            // hierarchical binding
            std::multimap<int, sc_export_base *> levels;
            std::set<int> maxLevel;
            vector<sc_export_base *>::iterator j;
            for (j = exports.begin(); j != exports.end(); ++j) {
                int l = countLevels(*j);
                maxLevel.insert(l);
                levels.insert(std::make_pair(l, *j));
            }
            // Check that all exports belong the the same module.
            string stem;
            size_t proxyCount = proxies.size();
            std::multimap<int, sc_export_base *>::iterator k;
            for (k = levels.begin(); k != levels.end(); ++k) {
                if (!compareStem(stem, k->second)) {
                    proxies.push_back(ifaceToObj(iface));
                    break;
                }
                stem = extractStem(k->second);
                if (k->first == 0) {
                    proxies.push_back(ifaceToObj(iface));
                    break;
                }
            }
            if (proxyCount != proxies.size())
                continue;

            // Find unique export on same level.
            std::set<int>::iterator l;
            for (l = maxLevel.begin(); l != maxLevel.end(); ++l) {
                if (levels.count(*l) == 1) {
                    ProxyInterface *proxy =
                        Proxy::findProxy(levels.find(*l)->second);
                    proxies.push_back(proxy ? proxy->simics_obj() : NULL);
                    break;
                }
            }
            if (proxyCount == proxies.size())
                proxies.push_back(ifaceToObj(iface));
        }
    }
    return proxies;
}

const char *ScPortConnection::if_typename() {
    return sc_core::simContextProxy::get_typename(base_);
}

int ScPortConnection::max_number_of_proxies() {
    return max_number_of_proxies_;
}

ScPortConnection::~ScPortConnection() {
    vector<Item>::iterator i;
    for (i = bind_info_.begin(); i!= bind_info_.end(); ++i) {
        sc_interface *iface = i->iface_;
        if (!iface)
            continue;

        keytype key = std::make_pair(iface, base_typename_);
        valuetype &v = ports_[key];
        v.erase(std::remove(v.begin(), v.end(), base_), v.end());
    }
}

int ScPortConnection::countLevels(sc_core::sc_object *object) {
    int levels = 0;
    string s = object->name();
    for (string::iterator i = s.begin(); i != s.end(); ++i)
        if (*i == '.')
            ++levels;

    return levels;
}

string ScPortConnection::extractStem(sc_core::sc_object *object) {
    string stem = object->name();
    size_t pos = stem.find_last_of('.');
    if (pos == string::npos)
        return "";

    return stem.substr(0, pos);
}

bool ScPortConnection::compareStem(string stem, sc_core::sc_object *object) {
    string s = extractStem(object);
    if (s.length() > stem.length())
        s.resize(stem.length());

    return stem == s;
}

conf_object_t *ScPortConnection::ifaceToObj(sc_core::sc_interface *iface) {
    ProxyInterface *proxy = Proxy::findProxy(iface);
    if (proxy)
        return proxy->simics_obj();
    else
        return ConnectionEndPoint::getEndPoint(simulation_, iface);
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
