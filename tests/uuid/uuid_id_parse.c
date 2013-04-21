#include "../tests.h"

int main()
{
  uuid_t uuid = { 0, 0 };
  char *test;
  int ret = 0;

  SIGCATCH_INIT

  test = "0123456789ABCDEF";
  ret = uuid_id_parse(&uuid, test);
  assert(ret == 0);
  assert((uuid.id >> 32)        == 0x01234567);
  assert((uuid.id & 0xFFFFFFFF) == 0x89ABCDEF);
  assert(uuid.dirname_hash == 0x0);

  return 0;
}
