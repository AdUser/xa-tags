#include "../tests.h"

int main()
{
  uuid_t uuid;
  char *test;
  int ret = 0;

  SIGCATCH_INIT

  uuid.id = 0;
  test = "/path/to/some/file";
  ret = uuid_generate(&uuid, test);
  assert(ret == 0);
  assert(uuid.id == 0);
  assert(uuid.dname == 0x5AE9);
  assert(uuid.fname == 0x8D35);

  uuid.id = 10;
  test = "/path/to/some/";
  ret = uuid_generate(&uuid, test);
  assert(ret == 0);
  assert(uuid.id == 10);
  assert(uuid.dname == 0x5AE9);
  assert(uuid.fname == 0xFFFF);

  uuid.id = 0;
  test = "/path/to/some";
  ret = uuid_generate(&uuid, test);
  assert(ret == 0);
  assert(uuid.id == 0);
  assert(uuid.dname == 0xED90);
  assert(uuid.fname == 0xAB46);

  return 0;
}
