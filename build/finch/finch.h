/*
 * finch
 *
 * Finch is the legal property of its developers, whose names are too numerous
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
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef FINCH_H
#define FINCH_H

#ifndef __GI_SCANNER__ /* hide this bit from g-ir-scanner */
# ifdef FINCH_COMPILATION
#  error "finch source files should not be including finch.h"
# endif /* FINCH_COMPILATION */
#endif /* __GI_SCANNER__ */

#ifndef FINCH_GLOBAL_HEADER_INSIDE
# define FINCH_GLOBAL_HEADER_INSIDE
#endif /* FINCH_GLOBAL_HEADER_INSIDE */

#include <finch/gntaccount.h>
#include <finch/gntblist.h>
#include <finch/gntconn.h>
#include <finch/gntconv.h>
#include <finch/gntdebug.h>
#include <finch/gntidle.h>
#include <finch/gntlog.h>
#include <finch/gntmedia.h>
#include <finch/gntmenuutil.h>
#include <finch/gntnotify.h>
#include <finch/gntplugin.h>
#include <finch/gntprefs.h>
#include <finch/gntrequest.h>
#include <finch/gntroomlist.h>
#include <finch/gntstatus.h>
#include <finch/gntui.h>
#include <finch/gntxfer.h>
#include <finch/libfinch.h>
#include <finch/finchenums.h>

#undef FINCH_GLOBAL_HEADER_INSIDE

#endif /* FINCH_H */
