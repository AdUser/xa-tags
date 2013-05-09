#include "../tests.h"

int main()
{
  char *test = "tag1 tag2 tag3";
  char *tags = NULL;

  SIGCATCH_INIT

  /* init */
  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);

  setxattr(TEST_FILE, XATTR_TAGS, test, strlen(test), 0);

  assert(file_tags_get_bulk(TEST_FILE, &tags) == 17);
  assert(memcmp(tags, " tag1 tag2 tag3 ", 14) == 0);

  /* cleanup */
  FREE(tags);
  unlink(TEST_FILE);

  return 0;
}
