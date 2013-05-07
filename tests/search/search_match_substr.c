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
  list_item_add(&terms, "mary", 0);
  list_item_add(&terms, "-bob", 0);

  ret = search_parse_terms(&search, &terms);
  assert(ret == 0);

  assert(search_match_substr(&search, "test1 test2 test3") == 0);
  assert(search_match_substr(&search, " alice mary ") == 1);
  assert(search_match_substr(&search, " alice rose ") == 0);
  assert(search_match_substr(&search, " alice mary bob") == 1);
  assert(search_match_substr(&search, " alice mary bob ") == 0);
  assert(search_match_substr(&search, " alice rosemary ") == 1);
  assert(search_match_substr(&search, " alice rosemary bob ") == 0);
  assert(search_match_substr(&search, " alice rosemary robert ") == 1);
  assert(search_match_substr(&search, " malice rosemary robert ") == 1);

  list_clear(&terms);
  search_free(&search);

  /* more complex examples */
  list_item_add(&terms, "alice", 0);
  assert(search_parse_terms(&search, &terms) == 0);
  assert(search_match_substr(&search, " alice ") == 1);
  assert(search_match_substr(&search, "malice") == 1);
  assert(search_match_substr(&search, " mary ") == 0);
  list_clear(&terms);
  search_free(&search);

  list_item_add(&terms, "~mary", 0);
  assert(search_parse_terms(&search, &terms) == 0);
  assert(search_match_substr(&search, " alice ") == 1);
  assert(search_match_substr(&search, " mary ") == 0);
  assert(search_match_substr(&search, "mary") == 0);
  assert(search_match_substr(&search, "rosemary") == 0);
  assert(search_match_substr(&search, " rosemary ") == 0);
  list_clear(&terms);
  search_free(&search);

  list_item_add(&terms, "+rose", 0);
  assert(search_parse_terms(&search, &terms) == 0);
  assert(search_match_substr(&search, "rose") == 0);
  assert(search_match_substr(&search, " rose ") == 1);
  assert(search_match_substr(&search, " rosemary ") == 0);
  assert(search_match_substr(&search, " maryrose ") == 0);
  assert(search_match_substr(&search, " alice rose") == 0);
  assert(search_match_substr(&search, " alice rose ") == 1);
  list_clear(&terms);
  search_free(&search);

  /* motto: nobody loves bob */
  list_item_add(&terms, "-bob", 0);
  assert(search_parse_terms(&search, &terms) == 0);
  list_idx_drop(&search->substr);
  list_idx_drop(&search->exact);
  assert(search_match_substr(&search, " alice mary rose bob ") == 0);
  assert(search_match_substr(&search, " alice mary rose bob") == 1);
  assert(search_match_substr(&search, " rose mary bob alice ") == 0);
  assert(search_match_substr(&search, " alice mary bobtail ") == 1);
  list_clear(&terms);
  search_free(&search);

  return 0;
}
