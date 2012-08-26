#include "../tests.h"

#define _GNU_SOURCE

int main()
{
  #ifndef _GNU_SOURCE
  unsigned char mem[22] = \
  {
    '\0', '\0',  't',  'e',  's',  't',  '1', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0',  't',  'e',  's',  't',
     '2', '\0'
  };
  unsigned char *ptr = NULL;
  #endif

  SIGCATCH_INIT

  #ifndef _GNU_SOURCE
  ptr = memmem(mem, 20, "test1", 5);
  assert(ptr == &mem[2]);

  ptr = memmem(mem, 20, "test2", 5);
  assert(ptr == NULL);

  ptr = memmem(mem, 21, "test2", 5);
  assert(ptr == &mem[16]);

  #endif
  return 0;
}
