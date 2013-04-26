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
_add_val_error(list_t *errors, char *error, char *buf, size_t buf_len)
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

  if ((ret = snprintf(t2, len, COMMON_ERR_FMT, error, t1)) > len)
    {
      len = ret + 1;
      goto again;
    }

  list_item_add(errors, t2, ret);
  FREE(t1);
  FREE(t2);

  return 1;
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_uuid(char *uuid, list_t *errors)
{
  if (uuid_id_validate(uuid) > 0)
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
_validate_path(char *path, list_t *errors)
{
  char *p = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);

  /* empty string */
  for (p = path; isblank(*p); p++);
  if (*p == '\0')
    return _add_val_error(errors, MSG_I_EXPPATH, path, 0);

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
_validate_tags(char *tags, list_t *errors)
{
  char  *p = NULL;

  ASSERT(tags != NULL, MSG_M_NULLPTR);

  /* empty string */
  for (p = tags; isblank(*p); p++);
  if (*p == '\0')
    return _add_val_error(errors, MSG_I_EXPTAGS, tags, 0);

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
  * 1 - empty list
  * 2 - error
  */
int
list_validate(list_t *list, list_t *errors, int strict)
 {
  char *item = NULL;
  int skip_item = 0;

  ASSERT(list != NULL, MSG_M_NULLPTR);

  if (list->type == LIST_EMPTY)
    {
      list_clear(list);
      return 0;
    }

  while(list_items_walk(list, &item) > 0)
    {
      switch (list->type)
        {
          case LIST_T_MSG :
            break;
          case LIST_L_FILES :
            if (_validate_path(item, errors)) skip_item = 1;
            break;
          case LIST_L_UUIDS :
            if (_validate_uuid(item, errors)) skip_item = 1;
            break;
          case LIST_L_TAGS :
            if (_validate_tags(item, errors)) skip_item = 1;
            break;
          case LIST_M_UUID_TAGS :
            if (_validate_uuid(item, errors) ||
                _validate_tags(&item[UUID_CHAR_LEN + 1], errors) ||
                !isblank(item[UUID_CHAR_LEN]))
                  skip_item = 1;
            break;
          case LIST_M_UUID_FILE :
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
          list_item_del(list, item);
          if ((item > list->buf) && (item < (list->buf + list->len)))
            item -= 2; /* TODO: hack around list_items_walk */
          skip_item = 0;
        }

      if ((list->items) == 0)
        {
          list_clear(list);
          return 1;
        }
    }

  if (list->items < 2 && list->flags & LIST_MULTI)
    list->flags &= ~LIST_MULTI;

  if (list->items > 1 && !(list->flags & LIST_MULTI))
    list->flags |= LIST_MULTI;

  /* if we have no items - exit point inside 'if (skip_item)' */

  return 0;
}

/** return values:
  * 0 - all ok
  * 1 - error occured
  *
  * item_len - optional, if <= 0 - use strlen(item)
  */
int
list_item_add(list_t *list, char *item, size_t item_len)
{
  char *t = NULL;
  const uint16_t chunk_size = 512;

  ASSERT(list != NULL, MSG_M_NULLPTR);

  if (item_len <= 0)
    item_len = strlen(item);

  item_len += 1; /* trailing '\0' */

  list->items += 1;

  /* check buffer size */
  if (item_len > list->size - list->len)
    {
      list->size += (item_len > chunk_size) ? item_len : chunk_size;

      REALLOC(t, list->buf, list->size);
      list->buf = t;
    }

  memcpy(&list->buf[list->len], item, item_len);
  list->buf[list->len + item_len - 1] = '\0';
  list->len += item_len;

  if (list->items > 2)
    list->flags |= LIST_MULTI;

  return 0;
}

/** return values:
 * 0 - nothing to do
 * 1 - deleted
 * 2 - error
 */
int
list_item_del(list_t *list, char *item)
{
  char *p = NULL;
  size_t item_len = 0;
  size_t move_size = 0;

  ASSERT(list != NULL && item != NULL, MSG_M_NULLPTR);

  if (list->items == 0)
    return 0; /* nothing to do */

  if ((p = list_item_search(list, item)) == NULL)
    return 0; /* nothing to do */

  item_len = strlen(item) + 1;

  move_size  = list->len;
  move_size -= p - list->buf;
  move_size -= item_len;
  memmove(p, p + item_len, move_size);

  list->len -= item_len;
  list->buf[list->len] = '\0';
  list->items -= 1;

  return 1;
}

void
list_items_split(list_t *list, char delim)
{
  size_t i = 0;

  for (i = 0; i < list->len; i++)
    if (list->buf[i] == delim)
      {
        list->buf[i] = '\0';
        list->items += 1;
       }
}

void
list_items_merge(list_t *list, char glue)
{
  size_t i = 0;
  size_t len = 0;

  if (list->items < 2)
    return;

  if (list->len > 0)
    len = list->len - 1;

  for (i = 0; i < len; i++)
    if (list->buf[i] == '\0')
      {
        list->buf[i] = glue;
        if (list->items > 1)
          list->items--;
       }
}

/** return values:
  * -1 - error occured
  *  0 - no more items
  *  1 - next item processed
  */
int
list_items_walk(const list_t *list, char **item)
{
  size_t len = 0;

  ASSERT(list != NULL && item != NULL, MSG_M_NULLPTR);

  if (list->len == 0)
    return 0;

  if (*item == NULL)
    {
      *item = list->buf;
      return 1;
    }

  if (*item < list->buf || *item > (list->buf + list->len))
    {
      *item = list->buf;
      return 1;
    }

  len = strlen(*item) + 1;

  if (*item + len >= list->buf + list->len)
    return 0;

  *item += len;

  return 1;
}

/** returns pointer to found item or NULL */
char *
list_item_search(list_t *list, const char *item)
{
  char *p = NULL;
  size_t item_len = 0;

  ASSERT(list != NULL && item != NULL, MSG_M_NULLPTR);

  if (list->items == 0)
    return NULL;

  if (strcmp(item, list->buf) == 0)
    return list->buf;

  item_len = strlen(item) + 1;
  p = list->buf;
  do {
    p = (char *) memmem(p, list->len - (p - list->buf), item, item_len);

    if (p == NULL)
      return NULL;

    if (p > list->buf && *(p - 1) == '\0')
      return p;

    p += item_len;
  } while (p != NULL);

  return NULL;
}

void
list_clear(list_t *list)
{
  ASSERT(list != NULL, MSG_M_NULLPTR);
  FREE(list->buf);
  memset(list, 0x0, sizeof(list_t));
}

void
list_copy(list_t *to, const list_t *from)
{
  memcpy(to, from, sizeof(list_t));
  to->buf = NULL;
  CALLOC(to->buf, from->size, sizeof(char));
  memcpy(to->buf, from->buf, from->len);
}

int
list_parse_tags(list_t *list, const char *tags)
{
  char *buf = NULL;
  char *s = NULL;
  char *e = NULL;
  size_t len = 0;

  ASSERT(list != NULL && tags != NULL, MSG_M_NULLPTR);

  STRNDUP(buf, tags, strlen(tags) + 1);

  list_clear(list);
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
          list_item_add(list, s, len);
          s += len;
        }
    }

  FREE(buf);
  return (list->items > 0) ? 1 : 0;
}

void
list_merge(list_t *to, list_t *from)
{
  char *p = NULL;
  char *item = NULL;

  ASSERT(to != NULL && from != NULL, MSG_M_NULLPTR);

  while (list_items_walk(from, &item) > 0)
    if ((p = list_item_search(to, item)) == NULL)
      list_item_add(to, item, 0);
}
