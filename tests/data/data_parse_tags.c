#include "../tests.h"

int main()
{
  data_t *data = NULL;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(data_t));

  data_parse_tags(data, "\t test1  test2 \t test3 ");
  assert(data->items == 3);
  assert(data->len == 18);
  assert(memcmp(data->buf, "test1\0test2\0test3\0", 18) == 0);

  return 0;
}
