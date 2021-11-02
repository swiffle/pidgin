/*
 * Copyright (C) 2011-2020 Gary Kramlich <grim@reaperworld.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */
#include <gplugin.h>
#include <gplugin-native.h>

G_MODULE_EXPORT GPluginPluginInfo *
gplugin_query(G_GNUC_UNUSED GError **error)
{
	return gplugin_plugin_info_new(
		"gplugin/native-unload-failed",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		NULL);
}

G_MODULE_EXPORT gboolean
gplugin_load(
	G_GNUC_UNUSED GPluginNativePlugin *plugin,
	G_GNUC_UNUSED GError **error)
{
	return TRUE;
}

G_MODULE_EXPORT gboolean
gplugin_unload(
	G_GNUC_UNUSED GPluginNativePlugin *plugin,
	G_GNUC_UNUSED gboolean shutdown,
	GError **error)
{
	g_set_error(error, GPLUGIN_DOMAIN, 0, "expected error");

	return FALSE;
}
