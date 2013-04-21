#include "../tests.h"

int main()
{
  uuid_t uuid_set = { 0, 0 };
  uuid_t uuid_get = { 0, 0 };

  SIGCATCH_INIT

  unlink(TEST_FILE);
  assert(creat(TEST_FILE, 0600) != -1);

  srand(time(NULL));

  uuid_set.id  = rand() & 0xFFFFFFFF;
  uuid_set.id <<= 32;
  uuid_set.id += rand() & 0xFFFFFFFF;

  assert(file_uuid_set(TEST_FILE, &uuid_set) == 0);
  assert(file_uuid_get(TEST_FILE, &uuid_get) == 0);

  assert(uuid_set.id == uuid_get.id);

  unlink(TEST_FILE);

  return 0;
}
