#include "../tests.h"

int main()
{
  list_t *list = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(list, 1, sizeof(list_t));

  /** init */
  list_item_add(list, "test1", 0);
  list_item_add(list, "test2", 0);
  list_item_add(list, "test3", 0);
  list_item_add(list, "test4", 0);

  ret = list_idx_create(list);
  /* sanity checks */
  assert(ret == 0);
  assert(list->idx_items != NULL);
  /* check strings */
  assert(memcmp(list->idx_items[0], "test1", 6) == 0);
  assert(memcmp(list->idx_items[1], "test2", 6) == 0);
  assert(memcmp(list->idx_items[2], "test3", 6) == 0);
  assert(memcmp(list->idx_items[3], "test4", 6) == 0);
  assert(list->idx_items[4] == NULL);
  /* check strlen */
  assert(list->idx_items_len[0] == 5);
  assert(list->idx_items_len[1] == 5);
  assert(list->idx_items_len[2] == 5);
  assert(list->idx_items_len[3] == 5);
  assert(list->idx_items_len[4] == 0);

  list_idx_drop(list);
  assert(list->idx_items == NULL);
  assert(list->idx_items_len == NULL);

  return 0;
}
