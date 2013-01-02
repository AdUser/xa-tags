#include "../tests.h"

int main()
{
  char *test = "tag1 tag2 tag3";

  SIGCATCH_INIT

  /* init */
  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);
  assert(setxattr(TEST_FILE, XATTR_TAGS, test, strlen(test), 0) == 0);

  /* test */
  assert(file_tags_clr(TEST_FILE) == 0);

  assert(getxattr(TEST_FILE, XATTR_TAGS, NULL, 0) == -1);
  assert(errno == ENOATTR);

  /* cleanup */
  unlink(TEST_FILE);

  return 0;
}
