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

/** return value:
  * must always return 1
  */
int
_add_val_error(data_t *errors, char *error, char *buf, size_t buf_len)
{
  char *t1 = NULL;
  char *t2 = NULL;
  size_t len = 0;
  int ret = 0;

  len = 1 + ((buf_len > 0) ? buf_len : strlen(buf));
  CALLOC(t1, len, sizeof(char));
  snprintf(t1, len, "%s", buf);

  len = 1024;

  again:
  FREE(t2);
  CALLOC(t2, len, sizeof(char));

  if ((ret = snprintf(t2, len, "%s -- %s", error, t1)) > len)
    {
      len = ret + 1;
      goto again;
    }

  data_item_add(errors, t2, ret);
  FREE(t1);
  FREE(t2);

  return 1;
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_uuid(char *uuid, data_t *errors)
{
  int i = 0;
  char c = '\0';

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  for (i = 0; i < 20; i++)
    {
      c = (isxdigit(uuid[i])) ? '0' : uuid[i];
      switch (c)
        {
          case '0' : if (i == 6 || i == 13) goto fail; break;
          case '-' : if (i != 6 && i != 13) goto fail; break;
          default  : /* and '\0' also */    goto fail; break;
        }
    }

  return 0;

  fail:
  _add_val_error(errors, MSG_I_BADUUID, uuid, 20);
  return 1;
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_path(char *path, data_t *errors)
{
  char *p = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);

  /* empty string */
  for (p = path; isblank(*p); p++);
  if (*p == '\0')
    {
      data_item_add(errors, MSG_I_EXPPATH, 0);
      return 1;
    }

  /* allowed paths must begin with '/' or '~/' */
  if (*p != '/' && strncmp("~/", p, 2) != 0)
    return _add_val_error(errors, MSG_I_BADPATH, path, 0);

  /* last char is '\' */
  p = strchr(p, '\0');
  if (p > path) p--;
  while (isspace(*p) && p > path) p--;
  if (*p == '\\')
    return _add_val_error(errors, MSG_I_BADCHARS, path, 0);

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - general error
  * 2 - unallowed symbol in tags
  */
int
_validate_tags(char *tags, data_t *errors)
{
  char  *p = NULL;

  ASSERT(tags != NULL, MSG_M_NULLPTR);

  /* empty string */
  for (p = tags; isblank(*p); p++);
  if (*p == '\0')
    {
      data_item_add(errors, MSG_I_EXPTAGS, 0);
      return 1;
    }

  /* quote in tags */
  for (p = tags; *p != '\0'; p++)
    if (*p == '\"')
      *p = '\''; /* silently fix it */

  /* last char is '\' */
  if (p > tags) p--;
  while (isspace(*p) && p > tags) p--;
  if (*p == '\\')
    return _add_val_error(errors, MSG_I_BADCHARS, tags, 0) + 1;

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - empty data set
  * 2 - error
  */
int
data_validate(data_t *data, data_t *errors, int strict)
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
            if (_validate_path(item, errors)) skip_item = 1;
            break;
          case DATA_L_UUIDS :
            if (_validate_uuid(item, errors)) skip_item = 1;
            break;
          case DATA_L_TAGS :
            if (_validate_tags(item, errors)) skip_item = 1;
            break;
          case DATA_M_UUID_TAGS :
            if (_validate_uuid(item, errors) ||
                _validate_tags(&item[21], errors) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          case DATA_M_UUID_FILE :
            if (_validate_uuid(item, errors) ||
                _validate_path(&item[21], errors) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          default :
            skip_item = 1;
            break;
        }
      if (skip_item)
        {
          if (strict) return 2; /* just save time */

          data->len -= item_len;
          memmove(item, item + item_len, data->len - read);
          t = realloc(data->buf, data->len);
          ASSERT(t != NULL, MSG_M_REALLOC);
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

  if (data->items == 0)
    {
      FREE(data->buf);
      memset(data, 0x0, sizeof(data_t));
      return 1;
    }

  return 0;
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
      CALLOC(data->buf, len + 1 + 1, sizeof(char));
      memcpy(data->buf, item, len);
      data->buf[len] = '\0';
      data->len += len + 1;
      data->items++;
      return 0;
    }
  else
    {
      t = realloc(data->buf, data->len + len + 1 + 1);
      ASSERT(t != NULL, MSG_M_REALLOC);
      data->buf = t;
      memcpy(&data->buf[data->len], item, len);
      memset(&data->buf[data->len + len], '\0', 2 * sizeof(char));
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
