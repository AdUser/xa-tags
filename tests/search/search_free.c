#include "../tests.h"

int main()
{
  int ret = 0;
  list_t terms;
  search_t search;

  SIGCATCH_INIT

  memset(&terms, 0x0, sizeof(list_t));
  memset(&search, 0x0, sizeof(search_t));

  list_item_add(&terms, "alice", 0);
  list_item_add(&terms, "mary", 0);
  list_item_add(&terms, "-bob", 0);
#ifdef REGEX_SEARCH
  list_item_add(&terms, "-/[ck]tulhu/", 0);
  list_item_add(&terms, "/nyan_cat/i", 0);
#endif

  search_parse_terms(&search, &terms);
  assert(ret == 0);

  list_clear(&terms);

  search_free(&search);
  assert(search.str_buf.items == 0);
  assert(search.str_buf.buf == NULL);
#ifdef REGEX_SEARCH
  assert(search.rxp_cnt == 0);
#endif

  return 0;
}
