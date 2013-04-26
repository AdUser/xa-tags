#include "../tests.h"

int main()
{
  list_t *data1 = NULL;
  list_t *data2 = NULL;
  list_t *data3 = NULL;
  int ret = 0;
  char *item = NULL;

  SIGCATCH_INIT

  CALLOC(data1, 1, sizeof(list_t));
  CALLOC(data2, 1, sizeof(list_t));
  CALLOC(data3, 1, sizeof(list_t));

  /** init list_t #1 */
  list_item_add(data1, "test1", 0);
  list_item_add(data1, "test12", 0);
  list_item_add(data1, "test123", 0);
  assert(data1->len == 21);
  assert(memcmp(data1->buf, "test1\0test12\0test123\0", 21) == 0);

  ret = list_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test1") == 0);

  ret = list_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test12") == 0);

  ret = list_items_walk(data1, &item);
  assert(ret == 1);
  assert(strcmp(item, "test123") == 0);

  ret = list_items_walk(data1, &item);
  assert(ret == 0);
  assert(strcmp(item, "test123") == 0);

  /** init list_t #2 */
  list_item_add(data2, "test3", 0);
  list_item_add(data2, "test32", 0);
  list_item_add(data2, "test321", 0);
  assert(data2->len == 21);
  assert(memcmp(data2->buf, "test3\0test32\0test321\0", 21) == 0);

  ret = list_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test3") == 0);

  ret = list_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test32") == 0);

  ret = list_items_walk(data2, &item);
  assert(ret == 1);
  assert(strcmp(item, "test321") == 0);

  ret = list_items_walk(data2, &item);
  assert(ret == 0);
  assert(strcmp(item, "test321") == 0);

  ret = list_items_walk(data3, &item);
  assert(ret == 0);

  return 0;
}
