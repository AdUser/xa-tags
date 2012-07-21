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
  * 0 - ok
  * 1 - error
  */
int
_validate_uuid(char *uuid)
{
  int i = 0;
  char c = '\0';

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  for (i = 0; i < 20; i++)
    {
      c = (isxdigit(uuid[i])) ? '0' : uuid[i];
      switch (c)
        {
          case '0' :
            if (i == 6 || i == 13) return 1;
            break;
          case '-' :
            if (i != 6 && i != 13) return 1;
            break;
          case 0x0 :
          default :
            return 1;
            break;
        }
    }

  return 0;
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_path(char *path)
{
  char *p = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);
  p = path;
  while (isblank(*p)) p++;

  if (*p == '\0')
    return 1; /* empty string */

  if (*p != '/' && strncmp("~/", p, 2) != 0)
    return 1; /* allowed paths must begin with '/' or '~/' */

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - general error
  * 2 - unallowed symbol in tags
  */
int
_validate_tags(char *tags)
{
  char  *p = NULL;

  ASSERT(tags != NULL, MSG_M_NULLPTR);

  for (p = tags; *p != '\0'; p++)
    if (*p == '\"')
      return 2;

  if (p > tags) p--;

  while (isspace(*p) || p >= tags) p--;

  if (*p == '\\') return 2;

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - empty data set
  * 2 - error
  */
int
data_validate(data_t *data)
 {
  char *t = NULL;
  char *item = NULL;
  size_t item_len = 0;
  int skip_item = 0;
  size_t read = 0;

  ASSERT(data != NULL, MSG_M_NULLPTR);

  if (data->type == DATA_EMPTY)
    {
      FREE(data->buf);
      data->len = 0;
      return 0;
    }

  item = data->buf;
  while(read < data->len)
    {
      item_len = strlen(item) + 1;
      switch (data->type)
        {
          case DATA_T_MSG :
            break;
          case DATA_L_FILES :
            if (_validate_path(item)) skip_item = 1;
            break;
          case DATA_L_UUIDS :
            if (_validate_uuid(item)) skip_item = 1;
            break;
          case DATA_M_UUID_TAGS :
            if (_validate_uuid(item) ||
                _validate_tags(&item[21]) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          case DATA_M_UUID_FILE :
            if (_validate_uuid(item) ||
                _validate_path(&item[21]) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          default :
            skip_item = 1;
            break;
        }
      if (skip_item)
        {
          data->len -= item_len;
          memmove(item, item + item_len, data->len - read);
          t = realloc(data->buf, data->len);
          if (t == NULL)
            {
              if (data->len == 0)
                {
                  data->type = DATA_EMPTY;
                  return 1;
                }
              /* TODO */
            }
          data->buf = t;
          item = &data->buf[read]; /* reset pointer after realloc() */
          skip_item = 0;
          continue;
        }
      data->items += 1;
      read += item_len;
      item = &data->buf[read];
    }

  if (data->items < 2 && data->flags & DATA_MULTI)
    data->flags &= ~DATA_MULTI;

  if (data->items > 1 && !(data->flags & DATA_MULTI))
    data->flags |= DATA_MULTI;

  return (data->items > 0) ? 0 : 1;
}

/**
 * item_len - optional, if 0 - use strlen(item)
 */
int
data_item_add(data_t *data, char *item, size_t item_len)
{
  size_t len = 0;
  char *t = NULL;

  ASSERT(data != NULL, MSG_M_NULLPTR);

  len = (item_len > 0) ? item_len : strlen(item);
  if (data->len == 0)
    {
      CALLOC(data->buf, len + 1, sizeof(char));
      memcpy(data->buf, item, len);
      data->buf[len] = '\0';
      data->len += len + 1;
      data->items++;
      return 0;
    } 
  else
    {
      t = realloc(data->buf, data->len + len + 1);
      ASSERT(t != NULL, MSG_M_REALLOC);
      data->buf = t;
      memcpy(&data->buf[data->len + 1], item, len);
      data->buf[data->len + len] = '\0';
      data->len += len + 1;
      data->items++;
      return 0;
    }
  return 1;
}

void
data_clear(data_t *data)
{
  ASSERT(data != NULL, MSG_M_NULLPTR);
  FREE(data->buf);
  memset(data, 0x0, sizeof(data_t));
}
