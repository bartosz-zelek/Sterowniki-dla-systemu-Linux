// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#ifndef SYSTEMC_CHECKPOINT_STATE_KEEPER_INTERFACE_H
#define SYSTEMC_CHECKPOINT_STATE_KEEPER_INTERFACE_H

#include <string>
#include <vector>

namespace sc_checkpoint {

/**
 * Users that need to handle their own state, for performance reasons, for
 * example, can implement this interface. The interface handles both in-memory
 * storage and writing state to disk.
 */

// <state-keeper-interface>
class StateKeeperInterface {
  public:
    virtual ~StateKeeperInterface() {}

    /**
     * State in the implementer is saved incrementally using this function. The
     * function must produce a handle which the library can use to restore or
     * merge with other in-memory states.
     */
    virtual bool save(void **handleOut) = 0;

    /**
     * Arbitrary state is restored in the implementer when this function is
     * invoked with a corresponding handle.
     */
    virtual bool restore(void *handle) = 0;

    /**
     * This function merges two states that are identified by the handles. The
     * contents of handleRemove are appended to handlePrevious, and
     * handleRemove is subsequently removed rendering the handle invalid.
     * NOTE: handlePrevious could be NULL, in which case handleRemove is just
     * removed
     */
    virtual bool merge(void *handlePrevious, void *handleRemove) = 0;

    /**
     * Write the state to disk. Dir is the directory in which state shall be
     * saved and prefix is a directory unique identifier. Persistent is a
     * boolean that indicates whether or not only persistent state should be
     * saved to disk. If persistent is not set all state is saved.
     */
    virtual bool write(const std::string &dir,
                       const std::string &prefix,
                       bool persistent) = 0;

    /**
     * Write a standalone checkpoint to disk. That is, the written checkpoint
     * cannot be dependent on any previous checkpoints. Dir is the directory in
     * which state shall be saved and prefix is a directory unique
     * identifier. Persistent is a boolean that indicates whether or not only
     * persistent state should be saved to disk. If persistent is not set all

     * state is saved.
     */
    virtual bool write_standalone(
        const std::string &dir, const std::string &prefix, bool persistent) = 0;

    /**
     * Restore state from disk. Dirs is a list of checkpoint directories and
     * prefix is an identifier.
     */
    virtual bool read(const std::vector<std::string> &dirs,
                      const std::string prefix, bool persistent) = 0;
};
// </state-keeper-interface>

}  // namespace sc_checkpoint

#endif
