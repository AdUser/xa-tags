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

/** return values:
 *  0 - if attribute not found or tag set is empty
 * >0 - on success
 */
ssize_t
file_tags_get(const char *path, char **tags)
{
  char *buf = 0;
  ssize_t size;

  errno = 0;

  /* determine needed buf size */
  size = getxattr(path, XATTR_TAGS, buf, 0);
  if (errno != 0)
     {
      if (errno != ENOATTR)
        msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 0;
    }

  if (size == 0)
    return 0;

  CALLOC(buf, size, sizeof(char));

  getxattr(path, XATTR_TAGS, buf, size);
  if (errno != 0)
    {
      if (errno != ENOATTR)
        msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 0;
    }

  buf[size] = '\0';

  FREE(*tags);
  *tags = buf;

  return size;
}

/** return values:
 *  0 - on success
 *  1 - if error occured
 */
int
file_tags_set(const char *path, char *tags, size_t len)
{
  errno = 0;
  setxattr(path, XATTR_TAGS, tags, len, 0);

  if (errno != 0)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  return 0;
}
