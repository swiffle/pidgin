/**
 * @file msnutils.h Utility functions
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
#ifndef MSN_UTILS_H
#define MSN_UTILS_H

/*encode the str to RFC2047 style*/
char *msn_encode_mime(const char *str);

/**
 * Generate the Random GUID
 */
char *rand_guid(void);

/**
 * Encodes the spaces in a string
 *
 * @param str The string to be encoded.
 * @param buf The buffer to hold the encoded string.
 * @param len The maximum length (including NUL) to put in @buf.
 *
 * @return Whether @str was able to fit in @buf.
 */
gboolean
msn_encode_spaces(const char *str, char *buf, size_t len);

/**
 * Parses the MSN message formatting into a format compatible with Purple.
 *
 * @param mime     The mime header with the formatting.
 * @param pre_ret  The returned prefix string.
 * @param post_ret The returned postfix string.
 *
 * @return The new message.
 */
void msn_parse_format(const char *mime, char **pre_ret, char **post_ret);

/**
 * Parses the Purple message formatting (html) into the MSN format.
 *
 * @param html			The html message to format.
 * @param attributes	The returned attributes string.
 * @param message		The returned message string.
 *
 * @return The new message.
 */
void msn_import_html(const char *html, char **attributes, char **message);

/**
 * Parses a socket string.
 *
 * @param str		A host:port string.
 * @param ret_host 	Return string value of the host.
 * @param ret_port	Return integer value of the port.
 */
void msn_parse_socket(const char *str, char **ret_host, int *ret_port);

/**
 * Verify if the email is a vaild passport.
 *
 * @param passport 	The email
 *
 * @return True if it is a valid passport, else FALSE
 */
gboolean msn_email_is_valid(const char *passport);

/**
 * Handle MSN Challenge Computation
 * This algorithm references
 * http://imfreedom.org/wiki/index.php/MSN:NS/Challenges
 *
 * @param input 	Challenge input.
 * @param output 	Callenge output.
 */
void msn_handle_chl(char *input, char *output);

#endif /* MSN_UTILS_H */
