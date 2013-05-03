#include "../tests.h"

int main()
{
  int ret = 0;
  list_t tags;
  char buf[1024];

  SIGCATCH_INIT

  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);
  memset(&tags, 0x0, sizeof(list_t));

  list_item_add(&tags, "tag1", 0);
  list_item_add(&tags, "tag2", 0);
  list_item_add(&tags, "tag3", 0);

  assert(file_tags_set(TEST_FILE, &tags) == 0);
  ret = getxattr(TEST_FILE, XATTR_TAGS, buf, 1024);
  assert(ret == 14);
  assert(memcmp(buf, "tag1 tag2 tag3\0", 14) == 0);

  unlink(TEST_FILE);

  return 0;
}
