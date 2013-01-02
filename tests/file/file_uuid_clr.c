#include "../tests.h"

int main()
{
  char *test = "1D7CFBC9A518C4F9";

  SIGCATCH_INIT

  /* init */
  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);
  assert(setxattr(TEST_FILE, XATTR_UUID, test, strlen(test), 0) == 0);

  /* test */
  assert(file_uuid_clr(TEST_FILE) == 0);

  assert(getxattr(TEST_FILE, XATTR_UUID, NULL, 0) == -1);
  assert(errno == ENOATTR);

  /* cleanup */
  unlink(TEST_FILE);

  return 0;
}
