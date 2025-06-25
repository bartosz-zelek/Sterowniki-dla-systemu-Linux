/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_LIBLINK_H
#define SIMICS_DEVS_LIBLINK_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/log.h>
#include <simics/util/frags.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

/* <add id="liblink_api">
   <insert id="link_message_t DOC"/>
   <name index="true">link_message_t</name>
   </add> */

/* <add id="link_message_t DOC">
     <ndx>link_message_t</ndx>
     <doc>
       <doc-item name="NAME">link_message_t</doc-item>
       <doc-item name="SYNOPSIS">
         typedef struct link_message link_message_t;
       </doc-item>
       <doc-item name="DESCRIPTION">
         Generic part of a link message. This structure should always be the
         first member of the link message data structure, so that the link
         library can access the generic part with a simple cast.
       </doc-item>
       <doc-item name="EXAMPLE">
         The <class>datagram_link</class> example defines its link message in
         the following way:
         <insert id="dl_msg_t"/>
       </doc-item>
       <doc-item name="SEE ALSO">
         link_type_t,
         SIMLINK_init_message
       </doc-item>
     </doc>
   </add> */
typedef struct {
        atomic_counter_t refcount;
} link_message_t;

/* <add id="liblink_api">
   <name index="true">link_type_t</name>
   <insert id="link_type_t DOC"/>
   </add> */

/* <add id="link_type_t DOC">
     <ndx>link_type_t</ndx>
     <doc>
       <doc-item name="NAME">link_type_t</doc-item>
       <doc-item name="SYNOPSIS"><insert id="link_type_t def"/></doc-item>
       <doc-item name="DESCRIPTION">
          Functions to be defined by the specific link implementation.

          These functions can be classified in four groups:

          <dl>

          <dt>Message Links Manipulation</dt>

          <dd>The first five functions are related to the link-specific
          messages.

          All five functions can be called in any execution context and should
          be thread-safe. They all take the link object as argument, in case it
          contains information necessary to perform the operation. As the link
          object is shared between the cells in which it is connected, it
          should not be modified during execution. Mutable state should be kept
          in the endpoint objects instead.

          <fun>msg_to_attr()</fun> transforms the message <param>msg</param>
          into an <type>attr_value_t</type> value. It is used to checkpoint
          in-flight messages waiting to be delivered. The value returned will
          be passed unchanged as argument <param>attr</param> to
          <fun>msg_from_attr()</fun> when loading a checkpoint with pending
          link messages. Neither function is expected to return an error,
          although <fun>msg_from_attr()</fun> is allowed to return NULL when
          translating a message it does not care to restore. This can be useful
          to keep checkpoint compatibility with older versions of the same link
          that do not always have the same message protocol.

          Using the <class>datagram_link</class> as an example, the
          <class>datagram-link</class> message is defined as:

          <insert id="dl_msg_t"/>

          <fun>msg_to_attr()</fun> and <fun>msg_from_attr()</fun> are thus
          defined as:

          <insert id="dl_to_a"/>
          <insert id="dl_fr_a"/>

          <fun>free_msg()</fun> is called when the message <param>msg</param>
          has been delivered to all its destinations and is no longer
          needed. All memory allocated for <param>msg</param> is expected to be
          freed, including <param>msg</param> itself. The
          <class>datagram_link</class> defines <fun>free_msg()</fun> as:

          <insert id="dl_free"/>

          <fun>marshal()</fun> is called when the message <param>msg</param>
          should be transmitted over a distributed simulation. Its purpose is
          to serialize the message into a <type>frags_t</type>
          representation. Rather than returning the marshaled message,
          <fun>marshal()</fun> takes the <param>finish</param> and
          <param>finish_data</param> arguments, that it is expected to call
          once the message has been marshaled.

          The reason behind this mechanism is that it allows
          <fun>marshal()</fun> to perform its operations with a
          <type>frags_t</type> variable allocated on the stack, and thus to
          skip any heap allocation when sending the message. In case memory was
          allocated anyway, it should be freed just after <param>finish</param>
          has returned.

          <insert id="dl_mrsh"/>

          <fun>unmarshal()</fun> does the opposite of <fun>marshal()</fun>: it
          takes a serialized <type>frags_t</type> representation of the message
          called <param>data</param> and returns a newly allocated link
          message.

          <insert id="dl_unm"/>

          </dd>

          <dt>Endpoint Configuration</dt>

          <dd>Link endpoints are created as needed by the link
          component. Depending on how they are created, they may not know yet
          which device they are connected to, so it might not be possible yet,
          for example, to cache the device's communication interface in the
          endpoint's <fun>finalize()</fun> function. Additionally, there are
          cases where the device the endpoint talks to may be changed, such as
          when inserting a probe object to listen to the traffic.

          In all of these cases, the <fun>device_changed()</fun> callback will
          be called when the endpoint's device attribute is changed
          <em>and</em> the endpoint has reached to finalize phase. In that
          callback, the new device can be obtained via
          <fun>SIMLINK_endpoint_device()</fun> and additional operations, such
          as interface caching, can be safely performed. The old device the
          endpoint was connected to is provided for convenience as an argument
          to <fun>device_changed()</fun>.

          Note that if no device related operations are necessary, this
          callback may be left unimplemented.

          The <class>ser_link</class> implementation of
          <fun>device_changed</fun> is the following:

          <insert id="sl_dev_changed"/>

          </dd>

          <dt>Message Delivery</dt>

          <dd>Messages are delivered to the link by calling the
          <fun>deliver()</fun> function. The arguments of <fun>deliver()</fun>
          are the endpoint <param>ep</param> that received the message and the
          message <param>msg</param> itself. The implementation of
          <fun>deliver()</fun> is expected to call the correct device's
          function to deliver the message.

          Note that <fun>deliver()</fun> can be called in any execution context
          and should be thread-safe. The link object is shared between the
          cells in which it is connected, and should not be modified during
          execution. Mutable state should be kept in the endpoint objects
          instead.

          The <class>datagram_link</class> implementation of
          <fun>deliver()</fun> is the following:

          <insert id="dl_dlv"/>

          </dd>

          <dt>Configuration</dt>

          <dd>The last two functions of <type>link_type_t</type> are taking
          care of the link configuration itself. In the same way messages needs
          to be marshaled when sent over a network, the global link
          configuration needs to be agreed upon when running the simulation in
          several processes. 

          <fun>update_config_value()</fun> is called whenever a configuration
          parameter has been added or updated. The configuration parameter's
          name is provided as <param>key</param> and its new value as
          <param>value</param>, encoded as a <type>frags_t</type>.

          <fun>remove_config_value()</fun> is called whenever the configuration
          value <param>key</param> has been removed.

          The interpretation of the link configuration messages is link
          specific. The only configuration parameter that is defined by the
          link library itself is <var>goal_latency</var>. This is handled
          entirely internally, although with the same mechanism as exposed
          here. Configuration changes are initiated by the link objects
          themselves with the Link Library API functions
          <fun>SIMLINK_config_update_value()</fun> and
          <fun>SIMLINK_config_remove_value()</fun>.

          Note that the link object that initiates the configuration change is
          also called back via <fun>update_config_value()</fun> and
          <fun>remove_config_value()</fun>. Note also that the configuration
          changes may be buffered and sent later if they are initiated too soon
          for the configuration message to propagate.

          Configuration changes should only be initiated while in Global
          Context, so the two configuration functions above will only be called
          in Global Context. This allows them to modify properties of
          the link object itself without needing to care about thread safety.

          As an example, here is how <class>ser_link</class> defines these two
          functions. The serial link keeps track of all endpoints connected to
          it by saving their ID as a configuration parameter. It also uses a
          configurable buffer size.

          Finally, it is important to note that these two callbacks may be
          called from a non-execution thread. They should call the Simics API
          only via <fun>SIM_thread_safe_callback()</fun>. This includes calling
          the <fun>SIM_log_*</fun> functions.

          <insert id="sl_cfg_up"/>
          <insert id="sl_cfg_rm"/>

          </dd>

          </dl>

       </doc-item>
       <doc-item name="SEE ALSO">
         link_message_t,
         SIMLINK_init
       </doc-item>

     </doc>
   </add>
   <add-type id="link_type_t def"></add-type> */
typedef struct {
        attr_value_t (*msg_to_attr)(conf_object_t *link, 
                                    const link_message_t *msg);
        link_message_t *(*msg_from_attr)(conf_object_t *link, 
                                         attr_value_t attr);
        void (*free_msg)(conf_object_t *link, link_message_t *msg);

        void (*marshal)(conf_object_t *link, const link_message_t *msg,
                        void (*finish)(void *data, const frags_t *msg),
                        void *finish_data);
        link_message_t *(*unmarshal)(conf_object_t *link, 
                                     const frags_t *msg);

        void (*deliver)(conf_object_t *ep, const link_message_t *msg);

        void (*update_config_value)(conf_object_t *link, const char *key,
                                    const frags_t *value);
        void (*remove_config_value)(conf_object_t *link, const char *key);
        void (*device_changed)(conf_object_t *ep, conf_object_t *old_dev);
} link_type_t;

#define LINK_NULL_ID ((uint64)0)
#define LINK_BROADCAST_ID ((uint64)-1)

conf_object_t *SIMLINK_endpoint_link(const conf_object_t *ep);
uint64 SIMLINK_endpoint_id(const conf_object_t *ep);
conf_object_t *SIMLINK_endpoint_clock(const conf_object_t *ep);
bool SIMLINK_endpoint_is_device(const conf_object_t *ep);
conf_object_t *SIMLINK_endpoint_device(const conf_object_t *ep);
const char *SIMLINK_endpoint_port(const conf_object_t *ep);
const char *SIMLINK_endpoint_dev_name(const conf_object_t *ep,
                                      buffer_t scratch);

void SIMLINK_init_library(void);

void SIMLINK_init_message(link_message_t *msg);

void SIMLINK_register_class(conf_class_t *cls);

void SIMLINK_register_endpoint_class(conf_class_t *cls, const char *msg_type);
void SIMLINK_register_snoop_endpoint_class(conf_class_t *cls);

void SIMLINK_init(conf_object_t *obj, const link_type_t *type);
void SIMLINK_endpoint_init(conf_object_t *ep_obj, bool snoop);
void SIMLINK_finalize(conf_object_t *link);
void SIMLINK_endpoint_finalize(conf_object_t *ep);
void SIMLINK_endpoint_disconnect(conf_object_t *ep);
void SIMLINK_pre_delete(conf_object_t *obj);

conf_object_t *SIMLINK_snoop_endpoint_create(conf_class_t *cls,
                                             conf_object_t *link,
                                             conf_object_t *clock,
                                             attr_value_t attrs);

void SIMLINK_config_update_value(conf_object_t *link, const char *key, 
                                 const frags_t *value);
void SIMLINK_config_remove_value(conf_object_t *link, const char *key);

conf_object_t *SIMLINK_find_endpoint_by_id(conf_object_t *link, uint64 id);

void SIMLINK_send_message(conf_object_t *src_ep,
                          uint64 dst_id, link_message_t *msg);
void SIMLINK_send_message_multi(conf_object_t *src_ep, unsigned num_dsts,
                                const uint64 *dst_ids, link_message_t *msg);

#endif  /* ! PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
