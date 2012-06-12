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
msg(enum msglevel level, char *format, ...)
  {
    va_list ap;

    if ((opts.daemon.loglevel == log_quiet  && level <= msg_error)  || \
        (opts.daemon.loglevel == log_normal && level <= msg_status) || \
        (opts.daemon.loglevel == log_extra  && level <= msg_info)   || \
        (opts.daemon.loglevel == log_all    && level <= msg_debug))
      {
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
      }

    if (level <= msg_error)
      exit(EXIT_FAILURE);
  }

