#include "../tests.h"

int main()
{
  data_t *data1 = NULL;
  data_t *data2 = NULL;

  SIGCATCH_INIT

  CALLOC(data1, 1, sizeof(data_t));
  CALLOC(data2, 1, sizeof(data_t));

  data_item_add(data1, "test1", 0);
  data_item_add(data1, "test2", 0);
  assert(data1->len == 12);
  assert(memcmp(data1->buf, "test1\0test2\0", 12) == 0);

  data_copy(data2, data1);
  assert(data1->items == data2->items);
  assert(data1->type  == data2->type);
  assert(data1->len   == data2->len);
  assert(data1->size  == data2->size);
  assert(data1->buf   != data2->buf);
  assert(data2->buf   != NULL);
  assert(memcmp(data1->buf, data2->buf, data1->len) == 0);

  data_clear(data1);
  data_clear(data2);
  free(data1);
  free(data2);

  return 0;
}
