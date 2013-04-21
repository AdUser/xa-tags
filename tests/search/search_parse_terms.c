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

  ret = search_parse_terms(&search, &terms);
  assert(ret == 0);
  assert(search.str_flags[0] == SEARCH_NONE);
  assert(search.str_flags[1] == SEARCH_NONE);
  assert(search.str_flags[2] == SEARCH_NEG);
  assert(search.str_flags[3] == SEARCH_NONE);
  assert(search.str_buf.items == 3);
  assert(memcmp(search.str_buf.buf, "alice\0mary\0bob\0", 15) == 0);
#ifdef REGEX_SEARCH
  assert(search.rxp_cnt == 2);
  assert(search.rxp_flags[0] == SEARCH_NEG);
  assert(search.rxp_flags[1] == SEARCH_NONE);
  assert(search.rxp_flags[2] == SEARCH_NONE);
  assert(memcmp(&search.rxp_buf[0], &search.rxp_buf[2], sizeof(regex_t)) != 0);
  assert(memcmp(&search.rxp_buf[1], &search.rxp_buf[2], sizeof(regex_t)) != 0);
#endif

  data_clear(&terms);
  data_clear(&search.str_buf);
#ifdef REGEX_SEARCH
  regfree(&search.rxp_buf[0]);
  regfree(&search.rxp_buf[1]);
#endif

  return 0;
}
