#include "../tests.h"

int main()
{
  data_t *data = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(data, 1, sizeof(data_t));

  /** init */
  ret = data_item_add(data, "test1", 0);
  ret = data_item_add(data, "test2", 0);
  ret = data_item_add(data, "test3", 0);
  ret = data_item_add(data, "test4", 0);
  assert(data->len == 24);
  assert(memcmp(data->buf, "test1\0test2\0test3\0test4\0", 24) == 0);

  /** delete item from end */
  ret = data_item_del(data, "test4");
  assert(ret == 1); /* success */
  assert(data->items == 3);
  assert(data->len == 18);
  assert(memcmp(data->buf, "test1\0test2\0test3\0", 18) == 0);

  /** delete item from middle */
  ret = data_item_del(data, "test2");
  assert(ret == 1); /* success */
  assert(data->items == 2);
  assert(data->len == 12);
  assert(memcmp(data->buf, "test1\0test3\0", 12) == 0);

  /** try to delete nonexistent item */
  ret = data_item_del(data, "test2");
  assert(ret == 0); /* nothing done */
  assert(data->items == 2);
  assert(data->len == 12);
  assert(memcmp(data->buf, "test1\0test3\0", 12) == 0);

  /** delete item from beginning */
  ret = data_item_del(data, "test1");
  assert(ret == 1); /* success */
  assert(data->items == 1);
  assert(data->len == 6);
  assert(memcmp(data->buf, "test3\0", 6) == 0);

  /** delete last item in list */
  ret = data_item_del(data, "test3");
  assert(ret == 1); /* success */
  assert(data->items == 0);
  assert(data->len == 0);
  assert(memcmp(data->buf, "\0", 1) == 0);

  /** delete on empty list */
  ret = data_item_del(data, "test2");
  assert(ret == 0); /* nothing done */
  assert(data->items == 0);
  assert(data->len == 0);
  assert(memcmp(data->buf, "\0", 1) == 0);

  return 0;
}
