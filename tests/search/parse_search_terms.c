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

  ret = parse_search_terms(&search, &terms);
  assert(ret == 0);
  assert(search.strings_neg[0] == false);
  assert(search.strings_neg[1] == false);
  assert(search.strings_neg[2] == true);
  assert(search.strings_neg[3] == false);
  assert(search.strings_buf.items == 3);
  assert(memcmp(search.strings_buf.buf, "alice\0mary\0bob\0", 15) == 0);
#ifdef REGEX_SEARCH
  assert(search.regexps_cnt == 2);
  assert(search.regexps_neg[0] == true);
  assert(search.regexps_neg[1] == false);
  assert(search.regexps_neg[2] == false);
  assert(memcmp(&search.regexps_buf[0], &search.regexps_buf[2], sizeof(regex_t)) != 0);
  assert(memcmp(&search.regexps_buf[1], &search.regexps_buf[2], sizeof(regex_t)) != 0);
#endif

  data_clear(&terms);
  data_clear(&search.strings_buf);
#ifdef REGEX_SEARCH
  regfree(&search.regexps_buf[0]);
  regfree(&search.regexps_buf[1]);
#endif

  return 0;
}
