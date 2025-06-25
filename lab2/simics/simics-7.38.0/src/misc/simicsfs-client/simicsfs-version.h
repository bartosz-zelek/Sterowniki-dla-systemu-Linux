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

#ifndef SIMICSFS_VERSION_H
#define SIMICSFS_VERSION_H

/* Simicsfs client combined version.
   The client combined version is a number of type x.y-z where:
   x = SIMICSFS_MSG_PROT_VERSION_MAJOR
   y = SIMICSFS_MSG_PROT_VERSION_MINOR
   z = SIMICSFS_CLIENT_VERSION

   x an y are stepped at simicsfs protocol changes, see simicsfs-prot-msg.h.
   z is independent from x and y and is stepped every time the client is
   changed.

   x, y and z are send to simicsfs server.
   x.y-z is printed with the 'simicsfs-client -V' command.
*/
#define SIMICSFS_CLIENT_VERSION 2

#endif /* SIMICSFS_VERSION_H */
