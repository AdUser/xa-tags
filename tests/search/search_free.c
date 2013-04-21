#include "../tests.h"

int main()
{
  int ret = 0;
  data_t terms;
  search_t search;

  SIGCATCH_INIT

  memset(&terms, 0x0, sizeof(data_t));
  memset(&search, 0x0, sizeof(search_t));

  data_item_add(&terms, "alice", 0);
  data_item_add(&terms, "mary", 0);
  data_item_add(&terms, "-bob", 0);
#ifdef REGEX_SEARCH
  data_item_add(&terms, "-/[ck]tulhu/", 0);
  data_item_add(&terms, "/nyan_cat/i", 0);
#endif

  parse_search_terms(&search, &terms);
  assert(ret == 0);

  data_clear(&terms);

  search_free(&search);
  assert(search.strings_buf.items == 0);
  assert(search.strings_buf.buf == NULL);
#ifdef REGEX_SEARCH
  assert(search.regexps_cnt == 0);
#endif

  return 0;
}
