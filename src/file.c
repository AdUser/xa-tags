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
 * 0 - all ok
 * 1 - if attribute not found or empty
 */
ssize_t
file_tags_get(const char *path, data_t *tags)
{
  char *buf = 0;
  ssize_t size;
  data_t tmp;

  errno = 0;
  memset(&tmp, 0x0, sizeof(data_t));

  /* determine needed buf size */
  size = getxattr(path, XATTR_TAGS, buf, 0);
  if (errno != 0)
     {
      if (errno != ENOATTR)
        msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  if (size <= 0)
    return 1;

  CALLOC(buf, size, sizeof(char));

  getxattr(path, XATTR_TAGS, buf, size);
  if (errno != 0)
    {
      if (errno != ENOATTR)
        msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      FREE(buf);
      return 1;
    }

  buf[size - 1] = '\0';

  data_parse_tags(&tmp, buf);
  data_merge(tags, &tmp);
  data_clear(&tmp);
  FREE(buf);

  return 0;
}

/** return values:
 *  0 - on success
 *  1 - if error occured
 */
int
file_tags_set(const char *path, data_t *tags)
{
  errno = 0;

  data_items_merge(tags, ' ');

  if (tags->len > 0)
    setxattr(path, XATTR_TAGS, tags->buf, tags->len, 0);
  else
    removexattr(path, XATTR_TAGS);

  if (errno != 0)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  return 0;
}

/** return values:
 *  0 - on success
 *  1 - if error occured
 */
int
file_tags_clr(const char *path)
{
  errno = 0;
  removexattr(path, XATTR_TAGS);
  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  return 0;
}

/** return values:
 * 0 - all ok
 * 1 - uuid not found
 * 2 - error occured
 */
int
file_uuid_get(const char *path, uuid_t *uuid)
{
  char buf[64] = { 0 };

  ASSERT(path != NULL && uuid != NULL, MSG_M_NULLPTR);

  getxattr(path, XATTR_UUID, buf, 64);
  buf[64 - 1] = '\0';
  if (errno != 0 && errno == ENOATTR)
    return 1;

  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  if (uuid_validate(buf) == 1)
    {
      msg(msg_warn, "%s -- %s\n", path, MSG_I_BADUUID);
      memset(uuid, 0, sizeof(uuid_t));
      return 1;
    }

  uuid_parse(uuid, buf);

  return 0;
}

/** return values:
 *  0 - on success
 *  1 - if error occured
 */
int
file_uuid_set(const char *path, uuid_t *uuid)
{
  char *p = NULL;

  p = uuid_printf(uuid);

  errno = 0;
  setxattr(path, XATTR_UUID, p, UUID_CHAR_LEN, 0);

  FREE(p);

  if (errno != 0)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  return 0;
}

/** return values:
 *  0 - on success
 *  1 - if error occured
 */
int
file_uuid_clr(const char *path)
{
  errno = 0;
  removexattr(path, XATTR_UUID);
  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, "%s -- %s\n", path, strerror(errno));
      return 1;
    }

  return 0;
}
