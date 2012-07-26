#include "../tests.h"

int main()
{
  buf_t *buf = NULL;

  SIGCATCH_INIT

  CALLOC(buf, 1, sizeof(buf_t));

  /** without strlen */
  buf_extend(buf, "test1", 0);
  assert(buf->len == 5);
  assert(memcmp(buf->buf, "test1\0", 6) == 0);

  buf_extend(buf, "test2", 0);
  assert(buf->len == 10);
  assert(memcmp(buf->buf, "test1test2\0", 11) == 0);

  FREE(buf->buf);
  FREE(buf);

  CALLOC(buf, 1, sizeof(buf_t));

  /** with strlen */
  buf_extend(buf, "test1", 3);
  assert(buf->len == 3);
  assert(memcmp(buf->buf, "tes\0", 4) == 0);

  buf_extend(buf, "test2", 5);
  assert(buf->len == 8);
  assert(memcmp(buf->buf, "testest2\0", 9) == 0);

  FREE(buf->buf);
  FREE(buf);

  return 0;
}
