/* spek-platform.h
 *
 * Copyright (C) 2010  Alexander Kojevnikov <alexander@kojevnikov.com>
 *
 * Spek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spek.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SPEK_PLATFORM_H__
#define __SPEK_PLATFORM_H__

#include <glib.h>

/* Convert from UTF-16 to UTF-8 when running on Windows */
void spek_platform_fix_args (gchar **argv, gint argc);

/* Open a link in the browser */
void spek_platform_show_uri (const gchar *uri);

#endif
