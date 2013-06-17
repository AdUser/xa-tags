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

/**
 @brief  get tags from file attribute and store them in list_t
 @param  path  path of file to handle
 @param  tags  struct to store retrieved tags
 @returns @b 0 on success,
          @b 1 if attribute not found or empty
 */
int
file_tags_get(const char *path, list_t *tags)
{
  char *buf = NULL;

  if (file_tags_get_bulk(path, &buf) <= 0)
    return 1;

  list_parse_tags(tags, buf);
  FREE(buf);

  return 0;
}

/**
 @brief  get tags from file attribute as single string

 @warning This function allocates a lot of memory. Don't forget to free() them.
 @param  path  path of file to handle
 @param  tags  store allocated string with tags to this pointer
 @returns  @b <0 on error,
           @b  0 if attribute not found or empty,
           @b >0 - length of allocated string
 */
ssize_t
file_tags_get_bulk(const char *path, char **tags)
{
  char *buf = NULL;
  ssize_t size;

  errno = 0;

  /* determine needed buf size */
  size = 1 + 2 + getxattr(path, XATTR_TAGS, buf, 0); /* '\0' + 2 x ' ' */
  if (errno != 0)
    {
      if (errno != ENOATTR)
        msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return -1;
    }

  if (size <= 3)
    return 0;

  CALLOC(buf, size, sizeof(char));

  getxattr(path, XATTR_TAGS, buf + 1, size - 1);
  if (errno != 0)
    {
      if (errno != ENOATTR)
        msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      FREE(buf);
      return -1;
    }

  buf[0] = ' ';
  buf[size - 2] = ' ';
  buf[size - 1] = '\0';
  *tags = buf;

  return size;
}

/**
 @brief  store set of tags into extended file attribute
 @param  path  path of file to handle
 @param  tags  list_t struct with tags to store in file's attribute
 @returns  @b 0 on success,
           @b 1 on error
 */
int
file_tags_set(const char *path, list_t *tags)
{
  errno = 0;

  if (tags->items < 1)
    {
      removexattr(path, XATTR_TAGS);
      return 0;
    }

  list_items_merge(tags, ' ');
  setxattr(path, XATTR_TAGS, tags->buf, tags->len - 1, 0);

  if (errno != 0)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return 1;
    }

  return 0;
}

/**
 @brief  clears and removes extended attribute with tags from file
 @param  path  path of file to handle
 @returns  @b 0 on success,
           @b 1 on error
 */
int
file_tags_clr(const char *path)
{
  errno = 0;
  removexattr(path, XATTR_TAGS);
  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return 1;
    }

  return 0;
}

/**
 @brief  get file's uuid from extended attribute
 @param  path  path of file to handle
 @returns  @b 0 on success,
           @b 1 if uuid not found,
           @b 2 if error occured
 */
int
file_uuid_get(const char *path, uuid_t *uuid)
{
  const size_t len = UUID_CHAR_LEN + 1;
  char buf[UUID_CHAR_LEN + 1] = { 0 };

  ASSERT(path != NULL && uuid != NULL, MSG_M_NULLPTR);

  errno = 0;
  getxattr(path, XATTR_UUID, buf, len);
  buf[len - 1] = '\0';
  if (errno != 0 && errno == ENOATTR)
    return 1;

  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return 2;
    }

  if (uuid_id_validate(buf) == 1)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, MSG_I_BADUUID);
      memset(uuid, 0, sizeof(uuid_t));
      return 2;
    }

  uuid_id_parse(uuid, buf);

  return 0;
}

/**
 @brief  store file's uuid to extended attribute
 @param  path  path of file to handle
 @returns  @b 0 on success,
           @b 1 on error
 */
int
file_uuid_set(const char *path, const uuid_t *uuid)
{
  errno = 0;
  setxattr(path, XATTR_UUID, uuid_id_printf(uuid), UUID_CHAR_LEN, 0);

  if (errno != 0)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return 1;
    }

  return 0;
}

/**
 @brief  clears and removes extended attribute with uuid from file
 @param  path  path of file to handle
 @returns  @b 0 on success,
           @b 1 on error
 */
int
file_uuid_clr(const char *path)
{
  errno = 0;
  removexattr(path, XATTR_UUID);
  if (errno != 0 && errno != ENOATTR)
    {
      msg(msg_warn, COMMON_ERR_FMTN, path, strerror(errno));
      return 1;
    }

  return 0;
}

/**
 @brief  Is specified file supports work with extended attributes?
 @param  path  path of file to test
 @returns  @b 0 if not supported,
           @b 1 if supported
 */
int
file_xattr_supported(const char *path)
{
  errno = 0;
  listxattr(path, XATTR_UUID, strlen(XATTR_UUID));

  return (errno == ENOTSUP) ? 0 : 1;
}
