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
  list_item_add(&terms, "/(alice|mary)/", 0);
  list_item_add(&terms, "-/(ro)*bert/i", 0);

  ret = search_parse_terms(&search, &terms);
  assert(ret == 0);

  assert(search_match_regex(&search, " alice rose ") == 1);
  assert(search_match_regex(&search, " Alice rose ") == 0);
  assert(search_match_regex(&search, " rose rosemary ") == 0);
  assert(search_match_regex(&search, " alice bob ") == 1); /* yay! */
  assert(search_match_regex(&search, " alice BeRt") == 0);
  assert(search_match_regex(&search, " rose bob ") == 0);
  assert(search_match_regex(&search, " alice Mary ") == 1);
  assert(search_match_regex(&search, " Alice rosemary ") == 0);

  list_clear(&terms);
  search_free(&search);

  return 0;
}
