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
 @brief helper  to handle validation error
 @param errors   bufffer for validation arrors
 @param error    error text
 @param buf      token, produced the error
 @param buf_len  length of the token
 @return always return 1
 */
static int
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

/**
 @brief   checks the uuid
 @param   uuid    checked uuid
 @param   errors  error messages buffer
 @return  0 - ok, 1 - malformed uuid or error
 */
static int
_validate_uuid(char *uuid, list_t *errors)
{
  if (uuid_id_validate(uuid) > 0)
    {
      _add_val_error(errors, MSG_I_BADUUID, uuid, UUID_CHAR_LEN);
      return 1;
    }

  return 0;
}

/**
 @brief   checks the path
 @param   path    checked path
 @param   errors  error messages buffer
 @return  0 - ok, 1 - malformed path or error
 */
static int
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

/**
 @brief   checks the tags
 @param   tags    checked tags
 @param   errors  error messages buffer
 @return  0 - ok, 1 - error, 2 - unallowed symbol in tags
 */
static int
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

/**
 @brief   validate provided list_t
 @detail  list->type must be set, otherwise this function frees all resources
          and threats list as empty
 @param  list    checked list
 @param  errors  error messages buffer
 @param  strict
 @return 0 - all ok, 1 - empty list, 2 - error
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

/**
 @brief   Adds item to list.
 @detail  Item_len are optional parameter.
          If set to zero - uses strlen() of item
 @param  list      append item to this list
 @param  item      item to be appended
 @param  item_len  optional length of provided item
 @return 0 - all ok, 1 - error
 */
int
list_item_add(list_t *list, char *item, size_t item_len)
{
  char *t = NULL;
  const uint16_t chunk_size = 512;

  ASSERT(list != NULL, MSG_M_NULLPTR);

  if (item_len <= 0)
    item_len = strlen(item);

  if (list->idx_items)
    list_idx_drop(list);

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

/**
 @brief  Deletes item from list
 @param  list  delete item from this list
 @param  item  item to be deleted
 @return  0 - item not found, 1 - successfull deletion, 2 - error
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

  if (list->idx_items)
    list_idx_drop(list);

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

/**
 @brief   Splits list item(s) to lesser size items.
 @detail  For example, list 'test' with single item "asd asd dsa",
          may be splitted to three items: "asd", "asd" and "dsa"
          by the following function call: list_items_split(test, ' ')
 @param  list   list to be processed
 @param  delim  use this char for items delimiter
 @return returns nothing
 */
void
list_items_split(list_t *list, char delim)
{
  size_t i = 0;

  if (list->idx_items)
    list_idx_drop(list);

  for (i = 0; i < list->len; i++)
    if (list->buf[i] == delim)
      {
        list->buf[i] = '\0';
        list->items += 1;
      }
}

/**
 @brief  Merges all list items to single, using char as glue
 @param  list   list to be processed
 @param  glue   use this char for glue
 @return returns nothing
 */
void
list_items_merge(list_t *list, char glue)
{
  size_t i = 0;
  size_t len = 0;

  if (list->items < 2)
    return;

  if (list->idx_items)
    list_idx_drop(list);

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

/**
 @brief   Sequentally processes list items
 @detail  If *item is NULL or not points to any member of this list
          this function set it to first point fo list.
 @param  list  list to be processed
 @param  item  pointer to next processed item
 @return -1 - error, 0 - no more items left, 1 - next item processed
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

/**
 @brief  Searches for item in list
 @param  list  Haystack.
 @param  item  Needle.
 @return  Return pointer to found item or NULL otherwise.
 */
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

/**
 @brief  Clear list and free all it's resources.
 @param  list  List to be cleared.
 */
void
list_clear(list_t *list)
{
  ASSERT(list != NULL, MSG_M_NULLPTR);
  FREE(list->buf);
  list_idx_drop(list);
  memset(list, 0x0, sizeof(list_t));
}

/**
 @brief  Makes copy of the list.
 @detail You must allocate 'to' list first!
 @param  to    New list.
 @param  from  List to be copyied.
 */
void
list_copy(list_t *to, const list_t *from)
{
  memcpy(to, from, sizeof(list_t));
  to->buf = NULL;
  to->idx_items = NULL;
  to->idx_items_len = NULL;
  CALLOC(to->buf, from->size, sizeof(char));
  memcpy(to->buf, from->buf, from->len);
}

/**
 @brief   Makes list_t from concatenated string of tags.
 @detail  Any existing data will be cleared from 'list'!
 @param  list  Where to store found tags.
 @param  tags  Source string of tags delimited by space.
 @return 0 - no tags found, 1 - all ok
 */
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

/**
 @brief   Adds missing items from one list to another.
 @param  to    Target list.
 @param  from  Source list.
 */
void
list_merge(list_t *to, list_t *from)
{
  char *p = NULL;
  char *item = NULL;

  if (to->idx_items)
    list_idx_drop(to);

  ASSERT(to != NULL && from != NULL, MSG_M_NULLPTR);

  while (list_items_walk(from, &item) > 0)
    if ((p = list_item_search(to, item)) == NULL)
      list_item_add(to, item, 0);
}

/**
 @brief  Deletes items index and free memory
 @param  list  List to be processed
 */
void
list_idx_drop(list_t *list)
{
  FREE(list->idx_items);
  FREE(list->idx_items_len);
}

/**
 @brief  Creates static index of items for fast access and compare.
 @param  list  List to be processed
 */
/** return values:
 * 0 - all ok
 * 1 - error
 */
int list_idx_create(list_t *list)
{
  char *item = NULL;
  int i = 0;

  ASSERT(list != NULL, MSG_M_NULLPTR);

  if (list->items == 0)
    return 0;

  CALLOC(list->idx_items, list->items + 1, sizeof(char *));
  CALLOC(list->idx_items_len, list->items + 1, sizeof(size_t));

  for (i = 0; list_items_walk(list, &item) > 0; i++)
  {
    list->idx_items[i]     = item;
    list->idx_items_len[i] = strlen(item);
  }

  return 0;
}
