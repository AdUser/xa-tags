/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "common.h"

#include "client/usage.inc.c"

enum loglevel verbosity = log_normal;

#if   OPERATIONAL_MODE ==  DUMB_TOOL
  #include "client/dumb_tool.inc.c"
#elif OPERATIONAL_MODE == SMART_TOOL
#elif OPERATIONAL_MODE ==  DUMB_CLIENT
#elif OPERATIONAL_MODE == SMART_CLIENT
#else
  /* do nothing */
#endif
