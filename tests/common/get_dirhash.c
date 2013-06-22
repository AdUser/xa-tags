#include "../tests.h"

int main()
{
  uint16_t dirhash = 0;
  char *test;

  SIGCATCH_INIT

  dirhash = 0;
  test = "/path/to/some/file";
  dirhash = get_dirhash(test);
  assert(dirhash == 0xEAD3);

  dirhash = 0;
  test = "/path/to/some/";
  dirhash = get_dirhash(test);
  assert(dirhash == 0xEAD3);

  dirhash = 0;
  test = "/path/to/some";
  dirhash = get_dirhash(test);
  assert(dirhash == 0xB6FF);

  return 0;
}
