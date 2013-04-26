#include "../tests.h"

int main()
{
  list_t tags;
  char *test = "tag1 tag2 tag3";

  SIGCATCH_INIT

  /* init */
  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);
  memset(&tags, 0x0, sizeof(list_t));

  /* test lowercase id */
  setxattr(TEST_FILE, XATTR_TAGS, test, strlen(test), 0);

  assert(file_tags_get(TEST_FILE, &tags) == 0);
  assert(memcmp(tags.buf, "tag1\0tag2\0tag3\0", tags.len) == 0);

  /* cleanup */
  unlink(TEST_FILE);

  return 0;
}
