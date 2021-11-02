/* Generated by wayland-scanner 1.18.0 */

#ifndef TABLET_UNSTABLE_V1_SERVER_PROTOCOL_H
#define TABLET_UNSTABLE_V1_SERVER_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-server.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct wl_client;
struct wl_resource;

/**
 * @page page_tablet_unstable_v1 The tablet_unstable_v1 protocol
 * Wayland protocol for graphics tablets
 *
 * @section page_desc_tablet_unstable_v1 Description
 *
 * This description provides a high-level overview of the interplay between
 * the interfaces defined this protocol. For details, see the protocol
 * specification.
 *
 * More than one tablet may exist, and device-specifics matter. Tablets are
 * not represented by a single virtual device like wl_pointer. A client
 * binds to the tablet manager object which is just a proxy object. From
 * that, the client requests wp_tablet_manager.get_tablet_seat(wl_seat)
 * and that returns the actual interface that has all the tablets. With
 * this indirection, we can avoid merging wp_tablet into the actual Wayland
 * protocol, a long-term benefit.
 *
 * The wp_tablet_seat sends a "tablet added" event for each tablet
 * connected. That event is followed by descriptive events about the
 * hardware; currently that includes events for name, vid/pid and
 * a wp_tablet.path event that describes a local path. This path can be
 * used to uniquely identify a tablet or get more information through
 * libwacom. Emulated or nested tablets can skip any of those, e.g. a
 * virtual tablet may not have a vid/pid. The sequence of descriptive
 * events is terminated by a wp_tablet.done event to signal that a client
 * may now finalize any initialization for that tablet.
 *
 * Events from tablets require a tool in proximity. Tools are also managed
 * by the tablet seat; a "tool added" event is sent whenever a tool is new
 * to the compositor. That event is followed by a number of descriptive
 * events about the hardware; currently that includes capabilities,
 * hardware id and serial number, and tool type. Similar to the tablet
 * interface, a wp_tablet_tool.done event is sent to terminate that initial
 * sequence.
 *
 * Any event from a tool happens on the wp_tablet_tool interface. When the
 * tool gets into proximity of the tablet, a proximity_in event is sent on
 * the wp_tablet_tool interface, listing the tablet and the surface. That
 * event is followed by a motion event with the coordinates. After that,
 * it's the usual motion, axis, button, etc. events. The protocol's
 * serialisation means events are grouped by wp_tablet_tool.frame events.
 *
 * Two special events (that don't exist in X) are down and up. They signal
 * "tip touching the surface". For tablets without real proximity
 * detection, the sequence is: proximity_in, motion, down, frame.
 *
 * When the tool leaves proximity, a proximity_out event is sent. If any
 * button is still down, a button release event is sent before this
 * proximity event. These button events are sent in the same frame as the
 * proximity event to signal to the client that the buttons were held when
 * the tool left proximity.
 *
 * If the tool moves out of the surface but stays in proximity (i.e.
 * between windows), compositor-specific grab policies apply. This usually
 * means that the proximity-out is delayed until all buttons are released.
 *
 * Moving a tool physically from one tablet to the other has no real effect
 * on the protocol, since we already have the tool object from the "tool
 * added" event. All the information is already there and the proximity
 * events on both tablets are all a client needs to reconstruct what
 * happened.
 *
 * Some extra axes are normalized, i.e. the client knows the range as
 * specified in the protocol (e.g. [0, 65535]), the granularity however is
 * unknown. The current normalized axes are pressure, distance, and slider.
 *
 * Other extra axes are in physical units as specified in the protocol.
 * The current extra axes with physical units are tilt, rotation and
 * wheel rotation.
 *
 * Since tablets work independently of the pointer controlled by the mouse,
 * the focus handling is independent too and controlled by proximity.
 * The wp_tablet_tool.set_cursor request sets a tool-specific cursor.
 * This cursor surface may be the same as the mouse cursor, and it may be
 * the same across tools but it is possible to be more fine-grained. For
 * example, a client may set different cursors for the pen and eraser.
 *
 * Tools are generally independent of tablets and it is
 * compositor-specific policy when a tool can be removed. Common approaches
 * will likely include some form of removing a tool when all tablets the
 * tool was used on are removed.
 *
 * Warning! The protocol described in this file is experimental and
 * backward incompatible changes may be made. Backward compatible changes
 * may be added together with the corresponding interface version bump.
 * Backward incompatible changes are done by bumping the version number in
 * the protocol and interface names and resetting the interface version.
 * Once the protocol is to be declared stable, the 'z' prefix and the
 * version number in the protocol and interface names are removed and the
 * interface version number is reset.
 *
 * @section page_ifaces_tablet_unstable_v1 Interfaces
 * - @subpage page_iface_zwp_tablet_manager_v1 - controller object for graphic tablet devices
 * - @subpage page_iface_zwp_tablet_seat_v1 - controller object for graphic tablet devices of a seat
 * - @subpage page_iface_zwp_tablet_tool_v1 - a physical tablet tool
 * - @subpage page_iface_zwp_tablet_v1 - graphics tablet device
 * @section page_copyright_tablet_unstable_v1 Copyright
 * <pre>
 *
 * Copyright 2014 © Stephen "Lyude" Chandler Paul
 * Copyright 2015-2016 © Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * </pre>
 */
struct wl_seat;
struct wl_surface;
struct zwp_tablet_manager_v1;
struct zwp_tablet_seat_v1;
struct zwp_tablet_tool_v1;
struct zwp_tablet_v1;

/**
 * @page page_iface_zwp_tablet_manager_v1 zwp_tablet_manager_v1
 * @section page_iface_zwp_tablet_manager_v1_desc Description
 *
 * An object that provides access to the graphics tablets available on this
 * system. All tablets are associated with a seat, to get access to the
 * actual tablets, use wp_tablet_manager.get_tablet_seat.
 * @section page_iface_zwp_tablet_manager_v1_api API
 * See @ref iface_zwp_tablet_manager_v1.
 */
/**
 * @defgroup iface_zwp_tablet_manager_v1 The zwp_tablet_manager_v1 interface
 *
 * An object that provides access to the graphics tablets available on this
 * system. All tablets are associated with a seat, to get access to the
 * actual tablets, use wp_tablet_manager.get_tablet_seat.
 */
extern const struct wl_interface zwp_tablet_manager_v1_interface;
/**
 * @page page_iface_zwp_tablet_seat_v1 zwp_tablet_seat_v1
 * @section page_iface_zwp_tablet_seat_v1_desc Description
 *
 * An object that provides access to the graphics tablets available on this
 * seat. After binding to this interface, the compositor sends a set of
 * wp_tablet_seat.tablet_added and wp_tablet_seat.tool_added events.
 * @section page_iface_zwp_tablet_seat_v1_api API
 * See @ref iface_zwp_tablet_seat_v1.
 */
/**
 * @defgroup iface_zwp_tablet_seat_v1 The zwp_tablet_seat_v1 interface
 *
 * An object that provides access to the graphics tablets available on this
 * seat. After binding to this interface, the compositor sends a set of
 * wp_tablet_seat.tablet_added and wp_tablet_seat.tool_added events.
 */
extern const struct wl_interface zwp_tablet_seat_v1_interface;
/**
 * @page page_iface_zwp_tablet_tool_v1 zwp_tablet_tool_v1
 * @section page_iface_zwp_tablet_tool_v1_desc Description
 *
 * An object that represents a physical tool that has been, or is
 * currently in use with a tablet in this seat. Each wp_tablet_tool
 * object stays valid until the client destroys it; the compositor
 * reuses the wp_tablet_tool object to indicate that the object's
 * respective physical tool has come into proximity of a tablet again.
 *
 * A wp_tablet_tool object's relation to a physical tool depends on the
 * tablet's ability to report serial numbers. If the tablet supports
 * this capability, then the object represents a specific physical tool
 * and can be identified even when used on multiple tablets.
 *
 * A tablet tool has a number of static characteristics, e.g. tool type,
 * hardware_serial and capabilities. These capabilities are sent in an
 * event sequence after the wp_tablet_seat.tool_added event before any
 * actual events from this tool. This initial event sequence is
 * terminated by a wp_tablet_tool.done event.
 *
 * Tablet tool events are grouped by wp_tablet_tool.frame events.
 * Any events received before a wp_tablet_tool.frame event should be
 * considered part of the same hardware state change.
 * @section page_iface_zwp_tablet_tool_v1_api API
 * See @ref iface_zwp_tablet_tool_v1.
 */
/**
 * @defgroup iface_zwp_tablet_tool_v1 The zwp_tablet_tool_v1 interface
 *
 * An object that represents a physical tool that has been, or is
 * currently in use with a tablet in this seat. Each wp_tablet_tool
 * object stays valid until the client destroys it; the compositor
 * reuses the wp_tablet_tool object to indicate that the object's
 * respective physical tool has come into proximity of a tablet again.
 *
 * A wp_tablet_tool object's relation to a physical tool depends on the
 * tablet's ability to report serial numbers. If the tablet supports
 * this capability, then the object represents a specific physical tool
 * and can be identified even when used on multiple tablets.
 *
 * A tablet tool has a number of static characteristics, e.g. tool type,
 * hardware_serial and capabilities. These capabilities are sent in an
 * event sequence after the wp_tablet_seat.tool_added event before any
 * actual events from this tool. This initial event sequence is
 * terminated by a wp_tablet_tool.done event.
 *
 * Tablet tool events are grouped by wp_tablet_tool.frame events.
 * Any events received before a wp_tablet_tool.frame event should be
 * considered part of the same hardware state change.
 */
extern const struct wl_interface zwp_tablet_tool_v1_interface;
/**
 * @page page_iface_zwp_tablet_v1 zwp_tablet_v1
 * @section page_iface_zwp_tablet_v1_desc Description
 *
 * The wp_tablet interface represents one graphics tablet device. The
 * tablet interface itself does not generate events; all events are
 * generated by wp_tablet_tool objects when in proximity above a tablet.
 *
 * A tablet has a number of static characteristics, e.g. device name and
 * pid/vid. These capabilities are sent in an event sequence after the
 * wp_tablet_seat.tablet_added event. This initial event sequence is
 * terminated by a wp_tablet.done event.
 * @section page_iface_zwp_tablet_v1_api API
 * See @ref iface_zwp_tablet_v1.
 */
/**
 * @defgroup iface_zwp_tablet_v1 The zwp_tablet_v1 interface
 *
 * The wp_tablet interface represents one graphics tablet device. The
 * tablet interface itself does not generate events; all events are
 * generated by wp_tablet_tool objects when in proximity above a tablet.
 *
 * A tablet has a number of static characteristics, e.g. device name and
 * pid/vid. These capabilities are sent in an event sequence after the
 * wp_tablet_seat.tablet_added event. This initial event sequence is
 * terminated by a wp_tablet.done event.
 */
extern const struct wl_interface zwp_tablet_v1_interface;

/**
 * @ingroup iface_zwp_tablet_manager_v1
 * @struct zwp_tablet_manager_v1_interface
 */
struct zwp_tablet_manager_v1_interface {
	/**
	 * get the tablet seat
	 *
	 * Get the wp_tablet_seat object for the given seat. This object
	 * provides access to all graphics tablets in this seat.
	 * @param seat The wl_seat object to retrieve the tablets for
	 */
	void (*get_tablet_seat)(struct wl_client *client,
				struct wl_resource *resource,
				uint32_t tablet_seat,
				struct wl_resource *seat);
	/**
	 * release the memory for the tablet manager object
	 *
	 * Destroy the wp_tablet_manager object. Objects created from
	 * this object are unaffected and should be destroyed separately.
	 */
	void (*destroy)(struct wl_client *client,
			struct wl_resource *resource);
};


/**
 * @ingroup iface_zwp_tablet_manager_v1
 */
#define ZWP_TABLET_MANAGER_V1_GET_TABLET_SEAT_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_manager_v1
 */
#define ZWP_TABLET_MANAGER_V1_DESTROY_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_seat_v1
 * @struct zwp_tablet_seat_v1_interface
 */
struct zwp_tablet_seat_v1_interface {
	/**
	 * release the memory for the tablet seat object
	 *
	 * Destroy the wp_tablet_seat object. Objects created from this
	 * object are unaffected and should be destroyed separately.
	 */
	void (*destroy)(struct wl_client *client,
			struct wl_resource *resource);
};

#define ZWP_TABLET_SEAT_V1_TABLET_ADDED 0
#define ZWP_TABLET_SEAT_V1_TOOL_ADDED 1

/**
 * @ingroup iface_zwp_tablet_seat_v1
 */
#define ZWP_TABLET_SEAT_V1_TABLET_ADDED_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_seat_v1
 */
#define ZWP_TABLET_SEAT_V1_TOOL_ADDED_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_seat_v1
 */
#define ZWP_TABLET_SEAT_V1_DESTROY_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_seat_v1
 * Sends an tablet_added event to the client owning the resource.
 * @param resource_ The client's resource
 * @param id the newly added graphics tablet
 */
static inline void
zwp_tablet_seat_v1_send_tablet_added(struct wl_resource *resource_, struct wl_resource *id)
{
	wl_resource_post_event(resource_, ZWP_TABLET_SEAT_V1_TABLET_ADDED, id);
}

/**
 * @ingroup iface_zwp_tablet_seat_v1
 * Sends an tool_added event to the client owning the resource.
 * @param resource_ The client's resource
 * @param id the newly added tablet tool
 */
static inline void
zwp_tablet_seat_v1_send_tool_added(struct wl_resource *resource_, struct wl_resource *id)
{
	wl_resource_post_event(resource_, ZWP_TABLET_SEAT_V1_TOOL_ADDED, id);
}

#ifndef ZWP_TABLET_TOOL_V1_TYPE_ENUM
#define ZWP_TABLET_TOOL_V1_TYPE_ENUM
/**
 * @ingroup iface_zwp_tablet_tool_v1
 * a physical tool type
 *
 * Describes the physical type of a tool. The physical type of a tool
 * generally defines its base usage.
 *
 * The mouse tool represents a mouse-shaped tool that is not a relative
 * device but bound to the tablet's surface, providing absolute
 * coordinates.
 *
 * The lens tool is a mouse-shaped tool with an attached lens to
 * provide precision focus.
 */
enum zwp_tablet_tool_v1_type {
	/**
	 * Pen
	 */
	ZWP_TABLET_TOOL_V1_TYPE_PEN = 0x140,
	/**
	 * Eraser
	 */
	ZWP_TABLET_TOOL_V1_TYPE_ERASER = 0x141,
	/**
	 * Brush
	 */
	ZWP_TABLET_TOOL_V1_TYPE_BRUSH = 0x142,
	/**
	 * Pencil
	 */
	ZWP_TABLET_TOOL_V1_TYPE_PENCIL = 0x143,
	/**
	 * Airbrush
	 */
	ZWP_TABLET_TOOL_V1_TYPE_AIRBRUSH = 0x144,
	/**
	 * Finger
	 */
	ZWP_TABLET_TOOL_V1_TYPE_FINGER = 0x145,
	/**
	 * Mouse
	 */
	ZWP_TABLET_TOOL_V1_TYPE_MOUSE = 0x146,
	/**
	 * Lens
	 */
	ZWP_TABLET_TOOL_V1_TYPE_LENS = 0x147,
};
#endif /* ZWP_TABLET_TOOL_V1_TYPE_ENUM */

#ifndef ZWP_TABLET_TOOL_V1_CAPABILITY_ENUM
#define ZWP_TABLET_TOOL_V1_CAPABILITY_ENUM
/**
 * @ingroup iface_zwp_tablet_tool_v1
 * capability flags for a tool
 *
 * Describes extra capabilities on a tablet.
 *
 * Any tool must provide x and y values, extra axes are
 * device-specific.
 */
enum zwp_tablet_tool_v1_capability {
	/**
	 * Tilt axes
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_TILT = 1,
	/**
	 * Pressure axis
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_PRESSURE = 2,
	/**
	 * Distance axis
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_DISTANCE = 3,
	/**
	 * Z-rotation axis
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_ROTATION = 4,
	/**
	 * Slider axis
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_SLIDER = 5,
	/**
	 * Wheel axis
	 */
	ZWP_TABLET_TOOL_V1_CAPABILITY_WHEEL = 6,
};
#endif /* ZWP_TABLET_TOOL_V1_CAPABILITY_ENUM */

#ifndef ZWP_TABLET_TOOL_V1_BUTTON_STATE_ENUM
#define ZWP_TABLET_TOOL_V1_BUTTON_STATE_ENUM
/**
 * @ingroup iface_zwp_tablet_tool_v1
 * physical button state
 *
 * Describes the physical state of a button that produced the button event.
 */
enum zwp_tablet_tool_v1_button_state {
	/**
	 * button is not pressed
	 */
	ZWP_TABLET_TOOL_V1_BUTTON_STATE_RELEASED = 0,
	/**
	 * button is pressed
	 */
	ZWP_TABLET_TOOL_V1_BUTTON_STATE_PRESSED = 1,
};
#endif /* ZWP_TABLET_TOOL_V1_BUTTON_STATE_ENUM */

#ifndef ZWP_TABLET_TOOL_V1_ERROR_ENUM
#define ZWP_TABLET_TOOL_V1_ERROR_ENUM
enum zwp_tablet_tool_v1_error {
	/**
	 * given wl_surface has another role
	 */
	ZWP_TABLET_TOOL_V1_ERROR_ROLE = 0,
};
#endif /* ZWP_TABLET_TOOL_V1_ERROR_ENUM */

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * @struct zwp_tablet_tool_v1_interface
 */
struct zwp_tablet_tool_v1_interface {
	/**
	 * set the tablet tool's surface
	 *
	 * Sets the surface of the cursor used for this tool on the given
	 * tablet. This request only takes effect if the tool is in
	 * proximity of one of the requesting client's surfaces or the
	 * surface parameter is the current pointer surface. If there was a
	 * previous surface set with this request it is replaced. If
	 * surface is NULL, the cursor image is hidden.
	 *
	 * The parameters hotspot_x and hotspot_y define the position of
	 * the pointer surface relative to the pointer location. Its
	 * top-left corner is always at (x, y) - (hotspot_x, hotspot_y),
	 * where (x, y) are the coordinates of the pointer location, in
	 * surface-local coordinates.
	 *
	 * On surface.attach requests to the pointer surface, hotspot_x and
	 * hotspot_y are decremented by the x and y parameters passed to
	 * the request. Attach must be confirmed by wl_surface.commit as
	 * usual.
	 *
	 * The hotspot can also be updated by passing the currently set
	 * pointer surface to this request with new values for hotspot_x
	 * and hotspot_y.
	 *
	 * The current and pending input regions of the wl_surface are
	 * cleared, and wl_surface.set_input_region is ignored until the
	 * wl_surface is no longer used as the cursor. When the use as a
	 * cursor ends, the current and pending input regions become
	 * undefined, and the wl_surface is unmapped.
	 *
	 * This request gives the surface the role of a cursor. The role
	 * assigned by this request is the same as assigned by
	 * wl_pointer.set_cursor meaning the same surface can be used both
	 * as a wl_pointer cursor and a wp_tablet cursor. If the surface
	 * already has another role, it raises a protocol error. The
	 * surface may be used on multiple tablets and across multiple
	 * seats.
	 * @param serial serial of the enter event
	 * @param hotspot_x surface-local x coordinate
	 * @param hotspot_y surface-local y coordinate
	 */
	void (*set_cursor)(struct wl_client *client,
			   struct wl_resource *resource,
			   uint32_t serial,
			   struct wl_resource *surface,
			   int32_t hotspot_x,
			   int32_t hotspot_y);
	/**
	 * destroy the tool object
	 *
	 * This destroys the client's resource for this tool object.
	 */
	void (*destroy)(struct wl_client *client,
			struct wl_resource *resource);
};

#define ZWP_TABLET_TOOL_V1_TYPE 0
#define ZWP_TABLET_TOOL_V1_HARDWARE_SERIAL 1
#define ZWP_TABLET_TOOL_V1_HARDWARE_ID_WACOM 2
#define ZWP_TABLET_TOOL_V1_CAPABILITY 3
#define ZWP_TABLET_TOOL_V1_DONE 4
#define ZWP_TABLET_TOOL_V1_REMOVED 5
#define ZWP_TABLET_TOOL_V1_PROXIMITY_IN 6
#define ZWP_TABLET_TOOL_V1_PROXIMITY_OUT 7
#define ZWP_TABLET_TOOL_V1_DOWN 8
#define ZWP_TABLET_TOOL_V1_UP 9
#define ZWP_TABLET_TOOL_V1_MOTION 10
#define ZWP_TABLET_TOOL_V1_PRESSURE 11
#define ZWP_TABLET_TOOL_V1_DISTANCE 12
#define ZWP_TABLET_TOOL_V1_TILT 13
#define ZWP_TABLET_TOOL_V1_ROTATION 14
#define ZWP_TABLET_TOOL_V1_SLIDER 15
#define ZWP_TABLET_TOOL_V1_WHEEL 16
#define ZWP_TABLET_TOOL_V1_BUTTON 17
#define ZWP_TABLET_TOOL_V1_FRAME 18

/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_TYPE_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_HARDWARE_SERIAL_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_HARDWARE_ID_WACOM_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_CAPABILITY_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_DONE_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_REMOVED_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_PROXIMITY_IN_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_PROXIMITY_OUT_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_DOWN_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_UP_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_MOTION_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_PRESSURE_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_DISTANCE_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_TILT_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_ROTATION_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_SLIDER_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_WHEEL_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_BUTTON_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_FRAME_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_SET_CURSOR_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_tool_v1
 */
#define ZWP_TABLET_TOOL_V1_DESTROY_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an type event to the client owning the resource.
 * @param resource_ The client's resource
 * @param tool_type the physical tool type
 */
static inline void
zwp_tablet_tool_v1_send_type(struct wl_resource *resource_, uint32_t tool_type)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_TYPE, tool_type);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an hardware_serial event to the client owning the resource.
 * @param resource_ The client's resource
 * @param hardware_serial_hi the unique serial number of the tool, most significant bits
 * @param hardware_serial_lo the unique serial number of the tool, least significant bits
 */
static inline void
zwp_tablet_tool_v1_send_hardware_serial(struct wl_resource *resource_, uint32_t hardware_serial_hi, uint32_t hardware_serial_lo)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_HARDWARE_SERIAL, hardware_serial_hi, hardware_serial_lo);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an hardware_id_wacom event to the client owning the resource.
 * @param resource_ The client's resource
 * @param hardware_id_hi the hardware id, most significant bits
 * @param hardware_id_lo the hardware id, least significant bits
 */
static inline void
zwp_tablet_tool_v1_send_hardware_id_wacom(struct wl_resource *resource_, uint32_t hardware_id_hi, uint32_t hardware_id_lo)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_HARDWARE_ID_WACOM, hardware_id_hi, hardware_id_lo);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an capability event to the client owning the resource.
 * @param resource_ The client's resource
 * @param capability the capability
 */
static inline void
zwp_tablet_tool_v1_send_capability(struct wl_resource *resource_, uint32_t capability)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_CAPABILITY, capability);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an done event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_tool_v1_send_done(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_DONE);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an removed event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_tool_v1_send_removed(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_REMOVED);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an proximity_in event to the client owning the resource.
 * @param resource_ The client's resource
 * @param tablet The tablet the tool is in proximity of
 * @param surface The current surface the tablet tool is over
 */
static inline void
zwp_tablet_tool_v1_send_proximity_in(struct wl_resource *resource_, uint32_t serial, struct wl_resource *tablet, struct wl_resource *surface)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_PROXIMITY_IN, serial, tablet, surface);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an proximity_out event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_tool_v1_send_proximity_out(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_PROXIMITY_OUT);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an down event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_tool_v1_send_down(struct wl_resource *resource_, uint32_t serial)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_DOWN, serial);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an up event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_tool_v1_send_up(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_UP);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an motion event to the client owning the resource.
 * @param resource_ The client's resource
 * @param x surface-local x coordinate
 * @param y surface-local y coordinate
 */
static inline void
zwp_tablet_tool_v1_send_motion(struct wl_resource *resource_, wl_fixed_t x, wl_fixed_t y)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_MOTION, x, y);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an pressure event to the client owning the resource.
 * @param resource_ The client's resource
 * @param pressure The current pressure value
 */
static inline void
zwp_tablet_tool_v1_send_pressure(struct wl_resource *resource_, uint32_t pressure)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_PRESSURE, pressure);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an distance event to the client owning the resource.
 * @param resource_ The client's resource
 * @param distance The current distance value
 */
static inline void
zwp_tablet_tool_v1_send_distance(struct wl_resource *resource_, uint32_t distance)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_DISTANCE, distance);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an tilt event to the client owning the resource.
 * @param resource_ The client's resource
 * @param tilt_x The current value of the X tilt axis
 * @param tilt_y The current value of the Y tilt axis
 */
static inline void
zwp_tablet_tool_v1_send_tilt(struct wl_resource *resource_, int32_t tilt_x, int32_t tilt_y)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_TILT, tilt_x, tilt_y);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an rotation event to the client owning the resource.
 * @param resource_ The client's resource
 * @param degrees The current rotation of the Z axis
 */
static inline void
zwp_tablet_tool_v1_send_rotation(struct wl_resource *resource_, int32_t degrees)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_ROTATION, degrees);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an slider event to the client owning the resource.
 * @param resource_ The client's resource
 * @param position The current position of slider
 */
static inline void
zwp_tablet_tool_v1_send_slider(struct wl_resource *resource_, int32_t position)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_SLIDER, position);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an wheel event to the client owning the resource.
 * @param resource_ The client's resource
 * @param degrees The wheel delta in 0.01 of a degree
 * @param clicks The wheel delta in discrete clicks
 */
static inline void
zwp_tablet_tool_v1_send_wheel(struct wl_resource *resource_, int32_t degrees, int32_t clicks)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_WHEEL, degrees, clicks);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an button event to the client owning the resource.
 * @param resource_ The client's resource
 * @param button The button whose state has changed
 * @param state Whether the button was pressed or released
 */
static inline void
zwp_tablet_tool_v1_send_button(struct wl_resource *resource_, uint32_t serial, uint32_t button, uint32_t state)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_BUTTON, serial, button, state);
}

/**
 * @ingroup iface_zwp_tablet_tool_v1
 * Sends an frame event to the client owning the resource.
 * @param resource_ The client's resource
 * @param time The time of the event with millisecond granularity
 */
static inline void
zwp_tablet_tool_v1_send_frame(struct wl_resource *resource_, uint32_t time)
{
	wl_resource_post_event(resource_, ZWP_TABLET_TOOL_V1_FRAME, time);
}

/**
 * @ingroup iface_zwp_tablet_v1
 * @struct zwp_tablet_v1_interface
 */
struct zwp_tablet_v1_interface {
	/**
	 * destroy the tablet object
	 *
	 * This destroys the client's resource for this tablet object.
	 */
	void (*destroy)(struct wl_client *client,
			struct wl_resource *resource);
};

#define ZWP_TABLET_V1_NAME 0
#define ZWP_TABLET_V1_ID 1
#define ZWP_TABLET_V1_PATH 2
#define ZWP_TABLET_V1_DONE 3
#define ZWP_TABLET_V1_REMOVED 4

/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_NAME_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_ID_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_PATH_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_DONE_SINCE_VERSION 1
/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_REMOVED_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_v1
 */
#define ZWP_TABLET_V1_DESTROY_SINCE_VERSION 1

/**
 * @ingroup iface_zwp_tablet_v1
 * Sends an name event to the client owning the resource.
 * @param resource_ The client's resource
 * @param name the device name
 */
static inline void
zwp_tablet_v1_send_name(struct wl_resource *resource_, const char *name)
{
	wl_resource_post_event(resource_, ZWP_TABLET_V1_NAME, name);
}

/**
 * @ingroup iface_zwp_tablet_v1
 * Sends an id event to the client owning the resource.
 * @param resource_ The client's resource
 * @param vid USB vendor id
 * @param pid USB product id
 */
static inline void
zwp_tablet_v1_send_id(struct wl_resource *resource_, uint32_t vid, uint32_t pid)
{
	wl_resource_post_event(resource_, ZWP_TABLET_V1_ID, vid, pid);
}

/**
 * @ingroup iface_zwp_tablet_v1
 * Sends an path event to the client owning the resource.
 * @param resource_ The client's resource
 * @param path path to local device
 */
static inline void
zwp_tablet_v1_send_path(struct wl_resource *resource_, const char *path)
{
	wl_resource_post_event(resource_, ZWP_TABLET_V1_PATH, path);
}

/**
 * @ingroup iface_zwp_tablet_v1
 * Sends an done event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_v1_send_done(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_V1_DONE);
}

/**
 * @ingroup iface_zwp_tablet_v1
 * Sends an removed event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
zwp_tablet_v1_send_removed(struct wl_resource *resource_)
{
	wl_resource_post_event(resource_, ZWP_TABLET_V1_REMOVED);
}

#ifdef  __cplusplus
}
#endif

#endif
