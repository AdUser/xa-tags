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

void
usage(int exitcode)
{
  fprintf(stdout,
  "%s  version %u.%u\n",
  PROGNAME, VERSION_MAJOR, VERSION_MINOR);

  fprintf(stdout,
  "Usage: %s [options] <file> [<file>]\n"
  "\n", PROGNAME);

  #include "client/usage_basic.inc.c"

  exit(exitcode);
}

enum loglevel verbosity = log_normal;

#define  DUMB_TOOL   1
#define SMART_TOOL   2
#define  DUMB_CLIENT 3
#define SMART_CLIENT 4

#if   MODE ==  DUMB_TOOL
  #include "client/dumb_tool.inc.c"
#elif MODE == SMART_TOOL
#elif MODE ==  DUMB_CLIENT
#elif MODE == SMART_CLIENT
#else
  /* do nothing */
#endif
