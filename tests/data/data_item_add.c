#include "../tests.h"

int main()
{
  data_t *data = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(data_t));

  /** add first item, test calloc() */
  ret = data_item_add(data, "test1", 0);
  assert(ret == 0);
  assert(data->len == 6);
  assert(memcmp(data->buf, "test1\0\0", 7) == 0);

  /** add second item, test realloc() */
  data_item_add(data, "test2", 0);
  assert(ret == 0);
  assert(data->len == 12);
  assert(memcmp(data->buf, "test1\0test2\0\0", 13) == 0);

  return 0;
}
