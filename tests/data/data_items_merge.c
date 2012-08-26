#include "../tests.h"

int main()
{
  data_t *data = NULL;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(data_t));

  data_item_add(data, "test1", 0);
  data_item_add(data, "test2", 0);
  data_item_add(data, "test3", 0);
  data_items_merge(data, '\n');
  assert(data->items == 1);
  assert(data->len == 18);
  assert(memcmp(data->buf, "test1\ntest2\ntest3\0\0", 19) == 0);

  return 0;
}
