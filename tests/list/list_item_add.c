#include "../tests.h"

int main()
{
  list_t *data = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(list_t));

  /** add first item, test calloc() */
  ret = list_item_add(data, "test1", 0);
  assert(ret == 0);
  assert(data->len == 6);
  assert(memcmp(data->buf, "test1\0", 6) == 0);

  /** add second item */
  list_item_add(data, "test2", 0);
  assert(ret == 0);
  assert(data->len == 12);
  assert(memcmp(data->buf, "test1\0test2\0", 12) == 0);

  list_clear(data);
  data->buf = calloc(10, sizeof(char));
  data->len = 0;
  data->size = 10;
  /** test buffer extending  */
  list_item_add(data, "very long string with spaces", 0);
  assert(data->size >= data->len);
  assert(data->len == 29);
  assert(memcmp(data->buf, "very long string with spaces", 29) == 0);

  return 0;
}
