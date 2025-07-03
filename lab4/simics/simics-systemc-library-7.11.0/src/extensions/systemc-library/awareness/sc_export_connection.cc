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

#include <simics/base/conf-object.h>
#include <simics/systemc/awareness/connection_end_point.h>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/sc_export_connection.h>
#include <simics/systemc/awareness/sc_port_connection.h>

#include <simics/systemc/sim_context_proxy.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {
bool sorter(sc_core::sc_export_base *i, sc_core::sc_export_base *j) {
    return strcmp(i->name(), j->name()) <= 0;
}
}  // namespace

namespace simics {
namespace systemc {
namespace awareness {

using sc_core::sc_interface;
using sc_core::sc_export_base;
using sc_core::sc_port_base;

std::map<ScExportConnection::keytype,
         ScExportConnection::valuetype> ScExportConnection::exports_;

ScExportConnection::ScExportConnection()
    : base_(NULL), iface_(NULL), levels_(0), simulation_(NULL) {
}

void ScExportConnection::init(sc_core::sc_object *object,
                              iface::SimulationInterface *simulation) {
    simulation_ = simulation;
    base_ = dynamic_cast<sc_export_base *>(object);
    if (!base_)
        return;

    iface_ = base_->get_interface();
    if (!iface_)
        return;

    keytype key = std::make_pair(iface_,
                                 sc_core::simContextProxy::get_typename(base_));
    insertKey(key);
    addBinderExport(key, base_);
    levels_ = countLevels(base_);
}

conf_object_t *ScExportConnection::export_to_proxy() {
    if (base_ == NULL)
        return NULL;

    std::set<sc_core::sc_export_base *> exports;
    std::multimap<int, sc_core::sc_export_base *> subModuleExports;

    std::set<keytype>::iterator i;
    for (i = keys_.begin(); i != keys_.end(); ++i) {
        valuetype &v = exports_[*i];
        for (valuetype::iterator j = v.begin(); j != v.end(); ++j) {
            if (*j == base_)
                continue;

            if (isPartOfModule(*j))
                exports.insert(*j);
        }
    }

    std::set<sc_core::sc_export_base *>::iterator j;
    std::set<int> maxLevel;
    for (j = exports.begin(); j != exports.end(); ++j) {
        int export_level = countLevels(*j);
        if (export_level > levels_) {
            maxLevel.insert(export_level);
            subModuleExports.insert(std::make_pair(export_level, *j));
        }
    }

    if (subModuleExports.size() == 0) {
        ProxyInterface *proxy = Proxy::findProxy(iface_);
        if (proxy)
            return proxy->simics_obj();

        return ConnectionEndPoint::getEndPoint(simulation_, iface_);
    }

    std::set<int>::iterator k;
    for (k = maxLevel.begin(); k != maxLevel.end(); ++k) {
        if (subModuleExports.count(*k) == 1) {
            ProxyInterface *proxy = Proxy::findProxy(
                subModuleExports.find(*k)->second);
            if (proxy)
                return proxy->simics_obj();

            return ConnectionEndPoint::getEndPoint(simulation_, iface_);
        }
    }

    return ConnectionEndPoint::getEndPoint(simulation_, iface_);
}

const char *ScExportConnection::if_typename() {
    return sc_core::simContextProxy::get_typename(base_);
}

void ScExportConnection::insertKey(keytype key) {
    keys_.insert(key);
}

std::vector<sc_core::sc_export_base *> ScExportConnection::findExport(
        keytype key) {
    return exports_[key];
}

void ScExportConnection::addBinderExport(const keytype &key,
                                         sc_export_base *obj) {
    valuetype &v = exports_[key];
    if (std::find(v.begin(), v.end(), obj) != v.end())
        return;

    v.push_back(obj);
    std::sort(v.begin(), v.end(), sorter);
}

void ScExportConnection::removeBinderExport(keytype key, sc_export_base *obj) {
    valuetype &v = exports_[key];
    v.erase(std::remove(v.begin(), v.end(), obj), v.end());
}

ScExportConnection::~ScExportConnection() {
    std::set<keytype>::iterator i;
    for (i = keys_.begin(); i != keys_.end(); ++i) {
        valuetype &v = exports_[*i];
        v.erase(std::remove(v.begin(), v.end(), base_), v.end());
    }
}

int ScExportConnection::countLevels(sc_core::sc_object *object) {
    int levels = 0;
    std::string s = object->name();
    for (std::string::iterator i = s.begin(); i != s.end(); ++i)
        if (*i == '.')
            ++levels;

    return levels;
}

bool ScExportConnection::isPartOfModule(sc_core::sc_object *object) {
    std::string stem = base_->name();
    std::string base = base_->basename();
    stem.resize(stem.size() - base.size());
    return stem.compare(0, std::string::npos, std::string(object->name()), 0,
                        stem.size()) == 0;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
