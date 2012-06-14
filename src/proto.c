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

/* name - required */
data_blk_t *
data_blk_create(char *name, unsigned int items)
  {
    size_t name_len = 0;
    data_blk_t *blk = NULL;

    name_len = (name != NULL) ? strlen(name) : 0;

    if (name_len == 0)
      {
        msg(msg_warn, _("Attempt to create data block with empty name\n"));
        return NULL;
      }

    if (name_len >= MAX_BLK_NAME_SIZE)
      {
        msg(msg_warn, _("Too long data blk name '%s',"
                        " max allowed: %u chars.\n"), name, MAX_BLK_NAME_SIZE);
        return NULL;
      }

    CALLOC(blk, 1, sizeof(data_blk_t));

    strncpy(blk->name, name, MAX_BLK_NAME_SIZE);

    if (items > 0)
      {
        blk->items_used  = 0;
        blk->items_total = items;
        CALLOC(blk->items, items, sizeof(data_item_t));
      }

    return blk;
  }

data_blk_t *
data_blk_extend(data_blk_t *blk, unsigned int num)
  {
    data_item_t *items = NULL;

    ASSERT(blk != NULL, MSG_M_NULLPTR);

    items = realloc(blk->items, sizeof(data_item_t) * \
                    (blk->items_total + num));

    ASSERT(items != NULL, MSG_M_REALLOC);

    blk->items = items;
    blk->items_total += num;
    memset(&blk->items[blk->items_used], 0x0, num * sizeof(data_item_t));

    return blk;
  }

data_blk_t *
data_blk_reduce(data_blk_t *blk, unsigned int num)
  {
    data_item_t *items = NULL;
    unsigned int n = 0;

    n = (blk->items_total >= num) ? num : blk->items_total ;

    ASSERT(blk != NULL, MSG_M_NULLPTR);

    items = realloc(blk->items, sizeof(data_item_t) * \
                    (blk->items_total - n));

    ASSERT(items != NULL, MSG_M_REALLOC);

    blk->items = items;
    blk->items_total -= n;

    return blk;
  }

/*
data_blk_t  *data_blk_item_add(data_blk_t *blk, data_item_t *item);
data_blk_t  *data_blk_item_del(data_blk_t *blk, char *key, bool all);

int ipc_data_blk_add(unsigned int *cnt, data_blk_t ***data);
int ipc_data_blk_del(unsigned int *cnt, char *blk_name);
*/
