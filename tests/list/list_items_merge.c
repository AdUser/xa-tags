#include "../tests.h"

int main()
{
  list_t *data = NULL;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(list_t));

  list_item_add(data, "test1", 0);
  list_item_add(data, "test2", 0);
  list_item_add(data, "test3", 0);
  list_items_merge(data, '\n');
  assert(data->items == 1);
  assert(data->len == 18);
  assert(memcmp(data->buf, "test1\ntest2\ntest3\0", 18) == 0);

  return 0;
}
