#include "../tests.h"

int main()
{
  buf_t *buf = NULL;

  SIGCATCH_INIT

  CALLOC(buf, 1, sizeof(buf_t));

  buf_extend(buf, "0123456789", 0);
  assert(buf->len == 10);
  assert(memcmp(buf->buf, "0123456789\0", 11) == 0);

  buf_reduce(buf, 3);
  assert(buf->len == 7);
  assert(memcmp(buf->buf, "3456789\0", 8) == 0);

  buf_reduce(buf, 7);
  assert(buf->len == 0);
  assert(memcmp(buf->buf, "\0", 1) == 0);

  FREE(buf->buf);
  FREE(buf);

  return 0;
}
