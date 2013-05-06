#include "common.h"

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
            /* NOTE: we also mix 'exact' string into substrings.    *
             * it helps to narrow number of results, but not for   *
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
            strncpy(buf, item + 1, item_len - ((icase == true) ? 2 : 1));
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

  return 0;
}

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
