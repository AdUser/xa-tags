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

  ret = search_parse_terms(&search, &terms);
  assert(ret == 0);
  assert(search.substr.items == 3);
  assert(search.exact.items  == 2);
  assert(memcmp(search.substr.buf, "+fred\0-velma\0+dafna\0", 6 + 7 + 9) == 0);
  assert(memcmp(search.exact.buf,  "+ dafna \0- shaggy \0", 9 + 10) == 0);
#ifdef REGEX_SEARCH
  assert(search.regex_cnt == 2);
  assert(search.regex_neg[0] == true);
  assert(search.regex_neg[1] == false);
  assert(search.regex_neg[2] == false);
  assert(memcmp(&search.regexps[0], &search.regexps[2], sizeof(regex_t)) != 0);
  assert(memcmp(&search.regexps[1], &search.regexps[2], sizeof(regex_t)) != 0);
#endif

  search_free(&search);

  return 0;
}
