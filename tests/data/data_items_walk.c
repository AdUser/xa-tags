#include "../tests.h"

int main()
{
  data_t *data1 = NULL;
  data_t *data2 = NULL;
  data_t *data3 = NULL;
  int ret = 0;
  char *item = NULL;

  SIGCATCH_INIT

  CALLOC(data1, 1, sizeof(data_t));
  CALLOC(data2, 1, sizeof(data_t));
  CALLOC(data3, 1, sizeof(data_t));

  /** init data_t #1 */
  data_item_add(data1, "test1", 0);
  data_item_add(data1, "test12", 0);
  data_item_add(data1, "test123", 0);
  assert(data1->len == 21);
  assert(memcmp(data1->buf, "test1\0test12\0test123\0\0", 22) == 0);

  ret = data_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test1") == 0);

  ret = data_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test12") == 0);

  ret = data_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test123") == 0);

  ret = data_items_walk(data1, &item);
  assert(ret == 0);
  assert(strcmp(item, "test123") == 0);

  /** init data_t #2 */
  data_item_add(data2, "test3", 0);
  data_item_add(data2, "test32", 0);
  data_item_add(data2, "test321", 0);
  assert(data2->len == 21);
  assert(memcmp(data2->buf, "test3\0test32\0test321\0\0", 22) == 0);

  ret = data_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test3") == 0);

  ret = data_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test32") == 0);

  ret = data_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test321") == 0);

  ret = data_items_walk(data2, &item);
  assert(ret == 0);
  assert(strcmp(item, "test321") == 0);

  ret = data_items_walk(data3, &item);
  assert(ret == 0);

  return 0;
}
