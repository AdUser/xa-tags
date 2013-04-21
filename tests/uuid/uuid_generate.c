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
  assert(uuid.dirname_hash == 0xEAD3);

  uuid.id = 10;
  test = "/path/to/some/";
  ret = uuid_generate(&uuid, test);
  assert(ret == 0);
  assert(uuid.id == 10);
  assert(uuid.dirname_hash == 0xEAD3);

  uuid.id = 0;
  test = "/path/to/some";
  ret = uuid_generate(&uuid, test);
  assert(ret == 0);
  assert(uuid.id == 0);
  assert(uuid.dirname_hash == 0xB6FF);

  return 0;
}
