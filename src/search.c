#include "common.h"

/**
 @brief  Parses string with search terms and fills search_t struct.
 @param  search  Struct to be filled.
 @param  terms  String with search terms.
 @returns  0 - all ok, 1 - error
 */
int
search_parse_terms(search_t * const search, const list_t *terms)
{
  char *item = NULL;
  size_t item_len = 0;
  size_t len = 0;
  char filter = '+'; /* '+', '-' */
  enum { substr, exact } match = substr;
  enum { undef, string, regexp } item_type = undef;
  char *buf;
  bool icase = false;
  int ret = 0;

  while (list_items_walk(terms, &item) > 0)
    {
      item_len = strlen(item);

      if (item_len == 0)
        msg(msg_error, COMMON_AST_FMT, __FILE__, __LINE__, MSG_U_UNKNERR);

      switch (*item)
        {
          default  : match = substr; filter = '+'; break;
          case '~' : match = substr; filter = '-'; item++; item_len--; break;
          case '+' : match = exact;  filter = '+'; item++; item_len--; break;
          case '-' : match = exact;  filter = '-'; item++; item_len--; break;
        }

      if ((item_len < 3 && *item != '/') ||
          (item_len < 4 && *item == '/'))
        {
          msg(msg_warn, COMMON_MSG_FMTN, MSG_S_SHORTTERM, item);
          continue;
        }

      /* detect type */
      if      (*item == '/' && item[item_len - 1] == '/')
        { item_type = regexp; icase = false; }
      else if (*item == '/' && strncmp(&item[item_len - 2], "/i", 2) == 0)
        { item_type = regexp; icase = true; }
      else
        { item_type = string; /* default */ }

      switch (item_type)
        {
          case string :
            if (search->substr.items >= MAX_SEARCH_COND)
              {        /* ^-- 'exact' not missed, see note */
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            CALLOC(buf, item_len + 10, sizeof(char));
            if (match == exact)
              {
                len = snprintf(buf, item_len + 10, "%c %s ", filter, item);
                list_item_add(&search->exact, buf, len);
              }
            /** @note: we also mix 'exact' string into substrings.  *
             * it helps to narrow number of results, but not for    *
             * *exclude* case, because it exclude more than needed. */
            if (match == substr || filter == '+')
              {
                len = snprintf(buf, item_len + 10, "%c%s", filter, item);
                list_item_add(&search->substr, buf, len);
              }
            FREE(buf);
            break;
          case regexp :
#ifdef REGEX_SEARCH
            if (search->regex_cnt >= MAX_SEARCH_COND)
              {
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            CALLOC(buf, item_len + 10, sizeof(char));
            search->regex_neg[search->regex_cnt] = (filter == '-') ? true : false;
            strncpy(buf, item + 1, item_len - ((icase == true) ? 3 : 2));
            if (icase == true)
              ret = regcomp(&search->regexps[search->regex_cnt], buf,
                            REG_EXTENDED | REG_NOSUB | REG_ICASE);
            else
              ret = regcomp(&search->regexps[search->regex_cnt], buf,
                            REG_EXTENDED | REG_NOSUB);
            FREE(buf);
            if (ret != 0)
              {
                len = regerror(ret, &search->regexps[search->regex_cnt], NULL, 0);
                CALLOC(buf, len, sizeof(char));
                regerror(ret, &search->regexps[search->regex_cnt], buf, len);
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_RXCOMPFAIL, buf);
                FREE(buf);
                return 1;
              }
            search->regex_cnt += 1;
#else
            msg(msg_warn, COMMON_MSG_FMTN, MSG_S_RXDISABLED, item);
            continue;
#endif
            break;
          case undef :
          default :
            msg(msg_error, COMMON_MSG_FMTN, MSG_S_UNKNTYPE, item);
            break;
        }
    }

  if (search->substr.items > 0)
    list_idx_create(&search->substr);

  if (search->exact.items > 0)
    list_idx_create(&search->exact);

  return 0;
}

/**
 @brief  Gracefully frees all allocated resources
 @param  search  Struct to be cleared.
 */
void
search_free(search_t * const search)
{
#ifdef REGEX_SEARCH
  uint8_t i = 0;

  for (i = 0; i < search->regex_cnt; i++)
    regfree(&search->regexps[i]);
#endif

  list_clear(&search->substr);
  list_clear(&search->exact);

  memset(search, 0x0, sizeof(search_t));
}

/**
 @internal
 @callergraph
 @brief  Matches terms against set of tags.
 @detail  If string @c tags contains all substrings from terms,
          and not contains any of substrings marked with 'negate' flag,
          the match considered successfull.
          This function for internal use only.
 @param  terms  List of terms.
 @param  tags   Tags string.
 @returns  -1 - error, 0 - not match, 1 - match
 */
static int
_match_against_list(const list_t *terms, const char *tags)
{
  int i = 0;
  char *p = NULL;
  bool found = false;
  bool expect = false;

  if (terms->idx_items == NULL)
    return -1;

  for (i = 0; i < terms->items; i++)
    {
      p = terms->idx_items[i];
      expect = (*p == '+') ? true : false;
      found =  (strstr(tags, (p + 1)) != NULL) ? true : false;
      if (!found && expect)
        return 0;
      if (found && !expect)
         return 0;
    }

  return 1;
}

/**
 @brief  Matches string @c tags against substrings in search_t
 @param  search  Search terms.
 @param  tags    Tags string.
 @returns  -1 - error, 0 - not match, 1 - match
 */
int
search_match_substr(const search_t *search, const char *tags)
{
  if (search->substr.items > 0)
    if (_match_against_list(&search->substr, tags) < 1)
      return 0;

  return 1;
}

/**
 @brief  Matches string @c tags against exact strings in search_t
 @param  search  Search terms.
 @param  tags    Tags string.
 @returns  -1 - error, 0 - not match, 1 - match
 */
int
search_match_exact(const search_t *search, const char *tags)
{
  if (search->exact.items > 0)
    if (_match_against_list(&search->exact, tags) < 1)
      return 0;

  return 1;
}

/**
 @brief  Matches string @c tags against regexps in search_t
 @param  search  Search terms.
 @param  tags    Tags string.
 @returns  -1 - error, 0 - not match, 1 - match
 */
#ifdef REGEX_SEARCH
int
search_match_regex(const search_t *search, const char *tags_str)
{
  int i = 0;
  int j = 0;
  list_t tags;
  bool match;
  bool expected;

  if (search->regex_cnt == 0)
    return 1;

  memset(&tags, 0x0, sizeof(list_t));

  if (list_parse_tags(&tags, tags_str) < 1)
    {
      msg(msg_warn, COMMON_MSG_FMTN, MSG_D_BADINPUT, tags_str);
      list_clear(&tags);
      return 0;
    }

  assert(list_idx_create(&tags) == 0);

  for (i = 0; i < search->regex_cnt; i++)
    {
      match = false;
      expected = !search->regex_neg[i];

      for (j = 0; j < tags.items; j++)
        if (regexec(&search->regexps[i], tags.idx_items[i], 0, NULL, 0) == 0)
          { match = true; break; }

      if ((match && !expected) ||
          (!match && expected))
        {
          list_clear(&tags);
          return 0;
        }
    }

  list_clear(&tags);

  return 1;
}
#endif

/**
 @brief  Top level function, combines substring, exact and regex match
 @param  search  Search terms.
 @param  tags    Tags string.
 @returns  0 - not match, 1 - match
 */
int
search_match(const search_t *search, const char *tags)
{
  if (search_match_substr(search, tags) < 1)
    return 0;

  if (search_match_exact(search, tags) < 1)
    return 0;

#ifdef REGEX_SEARCH
  if (search_match_regex(search, tags) < 1)
    return 0;
#endif

  return 1;
}
