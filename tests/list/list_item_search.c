#include "../tests.h"

int main()
{
  list_t *data = NULL;
  char *item1 = NULL;
  char *item2 = NULL;
  char *item3 = NULL;
  char *item4 = NULL;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(list_t));

  /** init list_t #1 */
  list_item_add(data, "test",   0);
  list_item_add(data, "string", 0);
  list_item_add(data, "ring",   0);
  assert(data->len == 17);
  assert(memcmp(data->buf, "test\0string\0ring\0", 17) == 0);

  item1 = list_item_search(data, "test");
  assert(item1 != NULL);
  assert(data->buf == item1);

  item2 = list_item_search(data, "string");
  assert(item2 != NULL);

  item3 = list_item_search(data, "ring");
  assert(item3 != NULL);
  assert(item3 != item2);

  item4 = list_item_search(data, "no such item");
  assert(item4 == NULL);

  return 0;
}
