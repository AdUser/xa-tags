#include "../tests.h"

int main()
{
  list_t *data1 = NULL;
  list_t *data2 = NULL;

  SIGCATCH_INIT

  CALLOC(data1, 1, sizeof(list_t));
  CALLOC(data2, 1, sizeof(list_t));

  list_item_add(data1, "test1", 0);
  list_item_add(data1, "test2", 0);
  assert(data1->len == 12);
  assert(memcmp(data1->buf, "test1\0test2\0", 12) == 0);

  list_item_add(data2, "test1", 0);
  list_item_add(data2, "test3", 0);
  assert(data2->len == 12);
  assert(memcmp(data2->buf, "test1\0test3\0", 12) == 0);

  list_merge(data1, data2);
  assert(data1->items == 3);
  assert(data1->len == 18);
  assert(memcmp(data1->buf, "test1\0test2\0test3\0", 18) == 0);

  return 0;
}
