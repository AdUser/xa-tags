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
  if (uuid_id_validate(uuid) == 1)
    {
      _add_val_error(errors, MSG_I_BADUUID, uuid, UUID_CHAR_LEN);
      return 1;
    }

  return 0;
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
  char *item = NULL;
  int skip_item = 0;

  ASSERT(data != NULL, MSG_M_NULLPTR);

  if (data->type == DATA_EMPTY)
    {
      data_clear(data);
      return 0;
    }

  while(data_items_walk(data, &item) > 0)
    {
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
                _validate_tags(&item[UUID_CHAR_LEN + 1], errors) ||
                !isblank(item[UUID_CHAR_LEN]))
                  skip_item = 1;
            break;
          case DATA_M_UUID_FILE :
            if (_validate_uuid(item, errors) ||
                _validate_path(&item[UUID_CHAR_LEN + 1], errors) ||
                !isblank(item[UUID_CHAR_LEN]))
                  skip_item = 1;
            break;
          default :
            skip_item = 1;
            break;
        }

      if (skip_item && strict)
        return 2; /* just save time */

      if (skip_item)
        {
          data_item_del(data, item);
          if ((item > data->buf) && (item < (data->buf + data->len)))
            item -= 2; /* TODO: hack around data_items_walk */
          skip_item = 0;
        }

      if ((data->items) == 0)
        {
          data_clear(data);
          return 1;
        }
    }

  if (data->items < 2 && data->flags & DATA_MULTI)
    data->flags &= ~DATA_MULTI;

  if (data->items > 1 && !(data->flags & DATA_MULTI))
    data->flags |= DATA_MULTI;

  /* if we have no items - exit point inside 'if (skip_item)' */

  return 0;
}

/** return values:
  * 0 - all ok
  * 1 - error occured
  *
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

int
data_item_del(data_t *data, char *item)
{
  char *p = NULL;
  size_t item_len = 0;

  ASSERT(data != NULL && item != NULL, MSG_M_NULLPTR);

  item_len = strlen(item) + 1;

  if (data->items == 0)
    return 0; /* nothing to do */

  if ((p = data_item_search(data, item)) == NULL)
    return 0; /* nothing to do */

  memmove(p, p + item_len, data->len - (p - data->buf) - item_len + 1);
  REALLOC(p, data->buf, data->len - item_len + 1);
  data->buf = p;

  data->len -= item_len;

  data->items -= 1;

  return 0;
}

void
data_items_split(data_t *data, char delim)
{
  size_t i = 0;

  for (i = 0; i < data->len; i++)
    if (data->buf[i] == delim)
      {
        data->buf[i] = '\0';
        data->items += 1;
       }
}

void
data_items_merge(data_t *data, char glue)
{
  size_t i = 0;
  size_t len = 0;

  if (data->len > 0)
    len = data->len - 1;

  for (i = 0; i < len; i++)
    if (data->buf[i] == '\0')
      {
        data->buf[i] = glue;
        if (data->items > 1)
          data->items--;
       }
}

/** return values:
  * -1 - error occured
  *  0 - no more items
  *  1 - next item processed
  */
int
data_items_walk(const data_t *data, char **item)
{
  size_t len = 0;

  ASSERT(data != NULL && item != NULL, MSG_M_NULLPTR);

  if (data->len == 0)
    return 0;

  if (*item == NULL)
    {
      *item = data->buf;
      return 1;
    }

  if (*item < data->buf || *item > (data->buf + data->len))
    {
      *item = data->buf;
      return 1;
    }

  len = strlen(*item) + 1;

  if (*item + len >= data->buf + data->len)
    return 0;

  *item += len;

  return 1;
}

/** returns pointer to found item or NULL */
char *
data_item_search(data_t *data, const char *item)
{
  char *p = NULL;
  size_t item_len = 0;

  ASSERT(data != NULL && item != NULL, MSG_M_NULLPTR);

  if (data->items == 0)
    return NULL;

  if (strcmp(item, data->buf) == 0)
    return data->buf;

  item_len = strlen(item) + 1;
  p = data->buf;
  do {
    p = (char *) memmem(p, data->len - (p - data->buf), item, item_len);

    if (p == NULL)
      return NULL;

    if (*(p - 1) == '\0')
      return p;

    p += item_len;
  } while (p != NULL);

  return NULL;
}

void
data_clear(data_t *data)
{
  ASSERT(data != NULL, MSG_M_NULLPTR);
  FREE(data->buf);
  memset(data, 0x0, sizeof(data_t));
}

int
data_parse_tags(data_t *data, const char *tags)
{
  char *buf = NULL;
  char *s = NULL;
  char *e = NULL;
  size_t len = 0;

  ASSERT(data != NULL && tags != NULL, MSG_M_NULLPTR);

  STRNDUP(buf, tags, strlen(tags) + 1);

  data_clear(data);
  s = buf;
  while (true)
    {
      while (isblank(*s)) s++;
      if (*s == '\0')
        break;
      e = s;
      while (*e != '\0' && isblank(*e) != 1) e++;
      len = e - s;
      if (len > 0)
        {
          data_item_add(data, s, len);
          s += len;
        }
    }

  FREE(buf);
  return (data->items > 0) ? 1 : 0;
}

void
data_merge(data_t *to, data_t *from)
{
  char *p = NULL;
  char *item = NULL;

  ASSERT(to != NULL && from != NULL, MSG_M_NULLPTR);

  while (data_items_walk(from, &item) > 0)
    if ((p = data_item_search(to, item)) == NULL)
      data_item_add(to, item, 0);
}
