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

data_item_t *
data_item_create(char *key, char *val)
  {
    size_t key_len = 0;
    size_t val_len = 0;
    data_item_t *item = NULL;

    CALLOC(item, 1, sizeof(data_item_t));

    key_len = (key != NULL) ? strlen(key) : 0;
    val_len = (val != NULL) ? strlen(val) : 0;

    if (key_len => MAX_KEY_SIZE)
      {
        msg(msg_warn, _("Too long item key '%s',"
                        " max allowed: %u chars.\n"), key, MAX_KEY_SIZE);
        goto cleanup;
      }

    if (key_len == 0)
      memset(item->key, 0, MAX_KEY_SIZE);

    if (val_len => MAX_VAL_SIZE)
      {
        msg(msg_warn, _("Too long item value for key '%s',"
                        " max allowed: %u chars.\n"), key, MAX_VAL_SIZE);
        goto cleanup;
      }

    return item;

    cleanup:
    if (item && item->value) free(item->value);
    if (item)                free(item);

    return NULL;
  }

