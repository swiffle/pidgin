/*
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef _PURPLE_TLS_CERTIFICATE_H
#define _PURPLE_TLS_CERTIFICATE_H
/**
 * SECTION:tls-certificate
 * @section_id: libpurple-tls-certificate
 * @short_description: <filename>tls-certificate.h</filename>
 * @title: TLS Certificate API
 */

#include <gio/gio.h>

/**
 * purple_tls_certificate_list_ids:
 *
 * Returns a list of the IDs for certificates trusted with
 * purple_tls_certificate_trust() and friends. These IDs can then be passed
 * to purple_certificate_path() or used directly, if desired.
 *
 * Returns: The #GList of IDs described above
 *          Free with purple_certificate_free_ids()
 */
GList *
purple_tls_certificate_list_ids(void);

/**
 * purple_tls_certificate_free_ids:
 * @ids: List of ids retrieved from purple_certificate_list_ids()
 *
 * Frees the list of IDs returned from purple_certificate_list_ids().
 */
void
purple_tls_certificate_free_ids(GList *ids);

/**
 * purple_tls_certificate_new_from_id:
 * @id: ID of certificate to load
 * @error: A GError location to store the error occurring, or NULL to ignore
 *
 * Loads the certificate referenced by ID into a #GTlsCertificate object.
 */
GTlsCertificate *
purple_tls_certificate_new_from_id(const gchar *id, GError **error);

/**
 * purple_tls_certificate_trust:
 * @id: ID to associate with the certificate
 * @certificate: Certificate to trust for TLS operations
 * @error: A GError location to store the error occurring, or NULL to ignore
 *
 * Trusts the certificate to be allowed for TLS operations even if
 * it would otherwise fail.
 *
 * Returns: #TRUE on success, #FALSE otherwise
 */
gboolean
purple_tls_certificate_trust(const gchar *id, GTlsCertificate *certificate,
		GError **error);

/**
 * purple_tls_certificate_distrust:
 * @id: ID associated with the certificate to distrust
 * @error: A GError location to store the error occurring, or NULL to ignore
 *
 * Revokes full trust of certificate. The certificate will be accepted
 * in TLS operations only if it passes normal validation.
 *
 * Returns: #TRUE on success, #FALSE otherwise
 */
gboolean
purple_tls_certificate_distrust(const gchar *id, GError **error);


/**
 * purple_tls_certificate_attach_to_tls_connection:
 * @conn: #GTlsConnection to connect to
 *
 * Connects the Purple TLS certificate subsystem to @conn so it will accept
 * certificates trusted by purple_tls_certificate_trust() and friends.
 *
 * Returns: @conn, similar to g_object_connect()
 */
gpointer
purple_tls_certificate_attach_to_tls_connection(GTlsConnection *conn);

/**
 * purple_tls_certificate_attach_to_socket_client:
 * @client: #GSocketClient to connect to
 *
 * Connects the Purple TLS certificate subsystem to @client so any TLS
 * connections it creates will accept certificates trusted by
 * purple_tls_certificate_trust() and friends.
 *
 * Returns: @client, similar to g_object_connect()
 */
gpointer
purple_tls_certificate_attach_to_socket_client(GSocketClient *client);


/**
 * PurpleTlsCertificateInfo
 *
 * An opaque structure to contain parsed certificate info, which
 * can subsequently be accessed by purple_tls_certificate_info_*
 * functions.
 */
typedef struct _PurpleTlsCertificateInfo PurpleTlsCertificateInfo;

/**
 * purple_tls_certificate_get_info:
 * @certificate: Certificate from which to parse the info
 *
 * Returns a #PurpleTlsCertificateInfo containing parsed information
 * of the certificate.
 *
 * Returns: #PurpleTlsCertificateInfo parsed from the certificate
 */
PurpleTlsCertificateInfo *
purple_tls_certificate_get_info(GTlsCertificate *certificate);

/**
 * purple_tls_certificate_info_free:
 * @info: #PurpleTlsCertificateInfo object to free
 *
 * Frees @info.
 */
void
purple_tls_certificate_info_free(PurpleTlsCertificateInfo *info);

/**
 * purple_tls_certificate_info_get_display_string:
 * @info: #PurpleTlsCertificateInfo from which to generate a display string
 *
 * Generates a user readable string to display information from @info
 *
 * Returns: A user readable string suitable to display to the user
 */
gchar *
purple_tls_certificate_info_get_display_string(PurpleTlsCertificateInfo *info);

/**
 * purple_tls_certificate_get_subject_name:
 * @certificate: Certificate from which to get the subject name
 *
 * Returns the common subject name of the cert
 *
 * Returns: The subject name of the cert
 */
gchar *
purple_tls_certificate_info_get_subject_name(PurpleTlsCertificateInfo *info);

/**
 * purple_tls_certificate_get_fingerprint_sha1:
 * @certificate: Certificate from which to get the SHA1 fingerprint
 *
 * Returns the SHA1 fingerprint of the cert
 *
 * Returns: The SHA1 fingerprint of the cert
 */
GByteArray *
purple_tls_certificate_get_fingerprint_sha1(GTlsCertificate *certificate);

G_END_DECLS

#endif /* _PURPLE_TLS_CERTIFICATE_H */
