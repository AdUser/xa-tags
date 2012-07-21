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
