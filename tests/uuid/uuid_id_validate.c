#include "../tests.h"

int main()
{
  int ret = 0;

  SIGCATCH_INIT

  ret = uuid_id_validate("99C029D606142B2E");
  assert(ret == 0); /* ok */

  ret = uuid_id_validate("99C029D606142B2");
  assert(ret == 1); /* err */

  ret = uuid_id_validate("99C029D606142B2 ");
  assert(ret == 1); /* err */

  ret = uuid_id_validate(" 99C029D606142B2");
  assert(ret == 1); /* err */

  ret = uuid_id_validate("99C029D6 06142B2");
  assert(ret == 1); /* err */

  ret = uuid_id_validate("99C029D6K6142B2E");
  assert(ret == 1); /* err */

  return 0;
}
