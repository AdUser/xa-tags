#include "../tests.h"

int main()
{
  int ret = 0;
  list_t terms;
  search_t search;

  SIGCATCH_INIT

  memset(&terms, 0x0, sizeof(list_t));
  memset(&search, 0x0, sizeof(search_t));

  list_item_add(&terms, "fred", 0);
  list_item_add(&terms, "~velma", 0);
  list_item_add(&terms, "+dafna", 0);
  list_item_add(&terms, "-shaggy", 0);
#ifdef REGEX_SEARCH
  list_item_add(&terms, "-/[ck]tulhu/", 0);
  list_item_add(&terms, "/nyan_cat/i", 0);
#endif

  search_parse_terms(&search, &terms);
  assert(ret == 0);

  list_clear(&terms);

  search_free(&search);
  assert(search.substr.items == 0);
  assert(search.substr.buf == NULL);
  assert(search.exact.items == 0);
  assert(search.exact.buf == NULL);
#ifdef REGEX_SEARCH
  assert(search.regex_cnt == 0);
#endif

  return 0;
}
