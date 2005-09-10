/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>

#include "dns_sd.h"
#include "bonjour.h"
#include "buddy.h"
#include "debug.h"

// Private data

typedef struct _dns_sd_packet{
	gchar* name;
	gchar* txtvers;
	gchar* version;
	gchar* first;
	gchar* last;
	gint port_p2pj;
	gchar* phsh;
	gchar* status;
	gchar* message;
	gchar* email;
	gchar* vc;
	gchar* jid;
	gchar* AIM;
}dns_sd_packet;

// End private data

// Private functions

static sw_result HOWL_API _publish_reply(sw_discovery discovery, 
	sw_discovery_oid oid, sw_discovery_publish_status status, sw_opaque extra)
{
gaim_debug_warning("bonjour", "_publish_reply --> Start\n");
	// Check the answer from the mDNS daemon
	switch(status){
		case SW_DISCOVERY_PUBLISH_STARTED :
			gaim_debug_info("bonjour", "_publish_reply --> Service started\n");
			break;
		case SW_DISCOVERY_PUBLISH_STOPPED :
			gaim_debug_info("bonjour", "_publish_reply --> Service stopped\n");
			break;
		case SW_DISCOVERY_PUBLISH_NAME_COLLISION :
			gaim_debug_info("bonjour", "_publish_reply --> Name collision\n");
			break;
		case SW_DISCOVERY_PUBLISH_INVALID :
			gaim_debug_info("bonjour", "_publish_reply --> Service invalid\n");
			break;
	}
	
	return SW_OKAY;
}

static sw_result HOWL_API _resolve_reply(sw_discovery discovery, 
	sw_discovery_oid oid, sw_uint32 interface_index, sw_const_string name, 
	sw_const_string type, sw_const_string domain, sw_ipv4_address address, 
	sw_port port, sw_octets text_record, sw_ulong text_record_len, 
	sw_opaque extra)
{
	BonjourBuddy* buddy;
	GaimAccount* account = (GaimAccount*)extra;
	gchar* txtvers = NULL;
	gchar* version = NULL;
	gchar* first = NULL;
	gint port_p2pj = -1;
	gchar* phsh = NULL;
	gchar* status = NULL;
	gchar* email = NULL;
	gchar* last = NULL;
	gchar* jid = NULL;
	gchar* AIM = NULL;
	gchar* vc = NULL;
	gchar* msg = NULL;
	gint address_length = 16;
	gchar* ip = NULL;
	sw_text_record_iterator iterator;
	sw_int8 key[SW_TEXT_RECORD_MAX_LEN];
	sw_int8 value[SW_TEXT_RECORD_MAX_LEN];
	sw_uint32 value_length;

	sw_discovery_cancel(discovery, oid);
	
	// Get the ip as a string
	ip = malloc(address_length);
	sw_ipv4_address_name(address, ip, address_length);

	// Obtain the parameters from the text_record
	if ((text_record_len > 0) && (text_record) && (*text_record != '\0')) {
		sw_text_record_iterator_init(&iterator, text_record, text_record_len);
		while (sw_text_record_iterator_next(iterator, key, value, &value_length) == SW_OKAY) {
			// Compare the keys with the possible ones and save them on 
			// the appropiate place of the buddy_list
			if (strcmp(key, "txtvers") == 0) {
				txtvers = g_strdup(value);
			} else if (strcmp(key, "version") == 0) {
				version = g_strdup(value);
			} else if (strcmp(key, "1st") == 0) {
				first = g_strdup(value);
			} else if (strcmp(key, "port.p2pj") == 0) {
				port_p2pj = atoi(value);
			} else if (strcmp(key, "status") == 0) {
				status = g_strdup(value);
			} else if (strcmp(key, "email") == 0) {
				email = g_strdup(value);
			} else if (strcmp(key, "last") == 0) {
				last = g_strdup(value);
			} else if (strcmp(key, "jid") == 0) {
				jid = g_strdup(value);
			} else if (strcmp(key, "AIM") == 0) {
				AIM = g_strdup(value);
			} else if (strcmp(key, "vc") == 0) {
				vc = g_strdup(value);
			} else if (strcmp(key, "phsh") == 0) {
				phsh = g_strdup(value);
			} else if (strcmp(key, "msg") == 0) {
				msg = g_strdup(value);
			}
		}
	}

	// Put the parameters of the text_record in a buddy and add the buddy to 
	// the buddy list
	buddy = bonjour_buddy_new((gchar*)name, first, port_p2pj, phsh, status, email,
		last, jid, AIM, vc, ip, msg);
		
	if (bonjour_buddy_check(buddy) == FALSE) {
		return SW_DISCOVERY_E_UNKNOWN;
	}
	
	/* Add or update the buddy in our buddy list */
	bonjour_buddy_add_to_gaim(account, buddy);
	
	// Free all the temporal strings
	g_free(txtvers);
	g_free(version);
	g_free(first);
	g_free(last);
	g_free(status);
	g_free(email);
	g_free(jid);
	g_free(AIM);
	g_free(vc);
	g_free(phsh);
	g_free(msg);
	
	return SW_OKAY;
}

static sw_result HOWL_API _browser_reply(sw_discovery discovery, sw_discovery_oid oid, 
			sw_discovery_browse_status status, sw_uint32 interface_index, sw_const_string name, 
			sw_const_string type, sw_const_string domain, sw_opaque_t extra)
{
	sw_discovery_resolve_id rid;
	GaimAccount* account = (GaimAccount*)extra;
	GaimBuddy* gb = NULL;
	
	switch(status){
		case SW_DISCOVERY_BROWSE_INVALID:
			gaim_debug_warning("bonjour", "_browser_reply --> Invalid\n");
			break;
		case SW_DISCOVERY_BROWSE_RELEASE:
			gaim_debug_warning("bonjour", "_browser_reply --> Release\n");
			break;
		case SW_DISCOVERY_BROWSE_ADD_DOMAIN:
			gaim_debug_warning("bonjour", "_browser_reply --> Add domain\n");
			break;
		case SW_DISCOVERY_BROWSE_ADD_DEFAULT_DOMAIN:
			gaim_debug_warning("bonjour", "_browser_reply --> Add default domain\n");
			break;
		case SW_DISCOVERY_BROWSE_REMOVE_DOMAIN:
			gaim_debug_warning("bonjour", "_browser_reply --> Remove domain\n");
			break;
		case SW_DISCOVERY_BROWSE_ADD_SERVICE:
			// A new peer has join the network and uses iChat bonjour
			gaim_debug_info("bonjour", "_browser_reply --> Add service\n");
			if (g_ascii_strcasecmp(name, account->username) != 0){
				if (sw_discovery_resolve(discovery, interface_index, name, type, 
						domain, _resolve_reply, extra, &rid) != SW_OKAY) {
					gaim_debug_warning("bonjour", "_browser_reply --> Cannot send resolve\n");
				}
			}
			break;
		case SW_DISCOVERY_BROWSE_REMOVE_SERVICE:
			gaim_debug_info("bonjour", "_browser_reply --> Remove service\n");
			gb = gaim_find_buddy((GaimAccount*)extra, name);
			if (gb != NULL) {
				bonjour_buddy_delete(gb->proto_data);
				gaim_blist_remove_buddy(gb);
			}
			break;
		case SW_DISCOVERY_BROWSE_RESOLVED:
			gaim_debug_info("bonjour", "_browse_reply --> Resolved\n");
			break;
		default:
			break;
	}
	
	return SW_OKAY;
}

int _dns_sd_publish(BonjourDnsSd* data, PublishType type)
{
	sw_text_record dns_data;
	sw_result publish_result;

	// Fill the data for the service	
	if(sw_text_record_init(&dns_data) != SW_OKAY){
		gaim_debug_error("bonjour", "Unable to initialize the data for the mDNS.");
		return -1;
	}

	sw_text_record_add_key_and_string_value(dns_data, "txtvers", data->txtvers);
	sw_text_record_add_key_and_string_value(dns_data, "version", data->version);
	sw_text_record_add_key_and_string_value(dns_data, "1st", data->first);
	sw_text_record_add_key_and_string_value(dns_data, "last", data->last);
	// sw_text_record_add_key_and_string_value(dns_data, "port.p2pj", itoa(data->port_p2pj));
	sw_text_record_add_key_and_string_value(dns_data, "port.p2pj", BONJOUR_DEFAULT_PORT);
	sw_text_record_add_key_and_string_value(dns_data, "phsh", data->phsh);
	sw_text_record_add_key_and_string_value(dns_data, "status", data->status);
	sw_text_record_add_key_and_string_value(dns_data, "msg", data->msg);
	sw_text_record_add_key_and_string_value(dns_data, "email", data->email);
	sw_text_record_add_key_and_string_value(dns_data, "vc", data->vc);
	sw_text_record_add_key_and_string_value(dns_data, "jid", data->jid);
	sw_text_record_add_key_and_string_value(dns_data, "AIM", data->AIM);

	// Publish the service
	switch (type) {
		case PUBLISH_START:
			publish_result = sw_discovery_publish(*(data->session), 0, data->name, ICHAT_SERVICE, NULL, 
								NULL, data->port_p2pj, sw_text_record_bytes(dns_data), sw_text_record_len(dns_data), 
								_publish_reply, NULL, &(data->session_id));
			break;
		case PUBLISH_UPDATE:
			publish_result = sw_discovery_publish_update(*(data->session),data->session_id,
								sw_text_record_bytes(dns_data), sw_text_record_len(dns_data));
			break;
	}
	if(publish_result != SW_OKAY){
		gaim_debug_error("bonjour", "Unable to publish or change the status of the _presence._tcp service.");
		return -1;
	}

	// Free the memory used by temp data
	sw_text_record_fina(dns_data);

	return 0;
}

gboolean _dns_sd_handle_packets(GIOChannel* source, GIOCondition condition, gpointer data)
{
	sw_discovery_read_socket(*((sw_discovery*)data));
	return TRUE;
}

gpointer _dns_sd_wait_for_connections(gpointer data)
{
	sw_discovery_oid session_id;
	BonjourDnsSd* dns_sd_data = (BonjourDnsSd*)data;

	// Advise the daemon that we are waiting for connections
	if(sw_discovery_browse(*(dns_sd_data->session), 0, ICHAT_SERVICE, NULL, _browser_reply,
			dns_sd_data->account, &session_id) != SW_OKAY){
		gaim_debug_error("bonjour", "Unable to get service.");
		return NULL;
	}

	// Yields control of the cpu to the daemon
	sw_discovery_run(*(dns_sd_data->session));
	
	return NULL;
}

// End private functions

/**
 * Allocate space for the dns-sd data.
 */
BonjourDnsSd* bonjour_dns_sd_new()
{
	BonjourDnsSd* data = g_new(BonjourDnsSd, 1);
	data->session = g_malloc(sizeof(sw_discovery));
	
	return data;
}

/**
 * Deallocate the space of the dns-sd data.
 */
void bonjour_dns_sd_free(BonjourDnsSd* data)
{
	g_free(data->session);
	g_free(data->first);
	g_free(data->last);
	g_free(data->email);
	g_free(data);
}

/**
 * Send a new dns-sd packet updating our status.
 */
void bonjour_dns_sd_send_status(BonjourDnsSd *data, const char *status, const char* status_message)
{
	g_free(data->status);
	g_free(data->msg);

	data->status = g_strdup(status);
	data->msg = g_strdup(status_message);

	// Update our text record with the new status
	_dns_sd_publish(data, PUBLISH_UPDATE); // <--We must control the errors
}

/**
 * Advertise our presence within the dns-sd daemon and start browsing
 * for other bonjour peers.
 */
void bonjour_dns_sd_start(BonjourDnsSd* data)
{	
	GIOChannel* io_channel;
	gint dns_sd_socket;
	sw_discovery_oid session_id;
	
	// Initilizations of the dns-sd data and session
	data->session = malloc(sizeof(sw_discovery));
	if(sw_discovery_init(data->session) != SW_OKAY){
		gaim_debug_error("bonjour", "Unable to initialize a mDNS session.");
		return;
	}
	
	// Publish our bonjour IM client at the mDNS daemon
	_dns_sd_publish(data, PUBLISH_START); // <--We must control the errors
	
	
	// Advise the daemon that we are waiting for connections
	if(sw_discovery_browse(*(data->session), 0, ICHAT_SERVICE, NULL, _browser_reply,
			data->account, &session_id) != SW_OKAY){
		gaim_debug_error("bonjour", "Unable to get service.");
		return;
	}
	
	// Get the socket that communicates with the mDNS daemon and bind it to a 
	// callback that will handle the dns_sd packets
	dns_sd_socket = sw_discovery_socket(*(data->session));
	io_channel = g_io_channel_unix_new(dns_sd_socket);
	g_io_add_watch(io_channel, G_IO_IN, _dns_sd_handle_packets, data->session); // Add more for other conditions like when the conn. has been broken
}

/**
 * Unregister the "_presence._tcp" service at the mDNS daemon.
 */
int bonjour_dns_sd_stop(BonjourDnsSd* data)
{
	sw_discovery_cancel(*(data->session), data->session_id);
	
	return 0;
}
