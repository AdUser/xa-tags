#include "../tests.h"

int main()
{
  uuid_t uuid  = 0;
  uint64_t expected = 0;
  char *test1 = "1d7cfbc9a518c4f9";
  char *test2 = "1D7CFBC9A518C4F9";

  SIGCATCH_INIT

  /* init */
  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);
  expected += 0x1D7CFBC9;
  expected <<= 32;
  expected += 0xA518C4F9;

  /* test lowercase id */
  setxattr(TEST_FILE, XATTR_UUID, test1, strlen(test1), 0);

  assert(file_uuid_get(TEST_FILE, &uuid) == 0);
  assert(uuid == expected);

  removexattr(TEST_FILE, XATTR_UUID);
  uuid = 0;

  /* test uppercase id */
  setxattr(TEST_FILE, XATTR_UUID, test2, strlen(test2), 0);

  assert(file_uuid_get(TEST_FILE, &uuid) == 0);
  assert(uuid == expected);

  removexattr(TEST_FILE, XATTR_UUID);
  uuid = 0;

  /* cleanup */
  unlink(TEST_FILE);

  return 0;
}
