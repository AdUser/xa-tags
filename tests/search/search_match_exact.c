#include "../tests.h"

int main()
{
  int ret = 0;
  list_t terms;
  search_t search;

  SIGCATCH_INIT

  memset(&terms, 0x0, sizeof(list_t));
  memset(&search, 0x0, sizeof(search_t));

  /* simple match */
  list_item_add(&terms, "alice", 0);
  list_item_add(&terms, "+mary", 0);
  list_item_add(&terms, "-bob", 0);

  ret = search_parse_terms(&search, &terms);
  assert(ret == 0);

  /* motto: nobody loves bob */
  assert(search_match_exact(&search, "test1 test2 test3") == 0);
  assert(search_match_exact(&search, " alice mary ") == 1);
  assert(search_match_exact(&search, " alice rose ") == 0);
  assert(search_match_exact(&search, " alice mary bob ") == 0);
  assert(search_match_exact(&search, " alice rosemary ") == 0);
  assert(search_match_exact(&search, " alice rosemary bob ") == 0);
  assert(search_match_exact(&search, " alice mary robert ") == 1);
  assert(search_match_exact(&search, " malice rosemary robert ") == 0);

  list_clear(&terms);
  search_free(&search);

  return 0;
}
