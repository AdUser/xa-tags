#include "common.h"

int
search_parse_terms(search_t * const search, const data_t *terms)
{
  int i = 0;
  char *item = NULL;
  char *flags = NULL;
  bool neg = false;
  size_t item_len = 0;
  data_t strings;
  enum { undef, string, regexp } type;
#ifdef REGEX_SEARCH
  char *buf = NULL;
  size_t buf_size = 4096;
  int regex_flags;
  int errcode = 0;
#endif

  memset(&strings, 0x0, sizeof(data_t));

  while (data_items_walk(terms, &item) > 0)
    {
      item_len = strlen(item);

      /* negation flag */
      neg = false;
      if (*item == '-')
        {
          item++;
          item_len--;
          neg = true;
        }

      /* detect type */
      if      (item_len > 3 && *item == '/' && item[item_len] == '/')
        { type = regexp; flags = &item[item_len]; }
      else if (item_len > 4 && *item == '/' && ((flags = strrchr(item + 1, '/')) != NULL))
        { type = regexp; }
      else
        { type = string; }

      switch (type)
        {
          case string :
            if (search->strings_buf.items >= MAX_SEARCH_COND)
              {
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            search->strings_neg[search->strings_buf.items] = neg;
            data_item_add(&search->strings_buf, item, item_len);
            break;
          case regexp :
#ifdef REGEX_SEARCH
            if (search->regexps_cnt >= MAX_SEARCH_COND)
              {
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            search->regexps_neg[search->regexps_cnt] = neg;
            buf_size = flags - item;
            CALLOC(buf, buf_size, sizeof(char));
            strncpy(buf, item + 1, flags - item - 1);
            regex_flags = REG_EXTENDED | REG_NOSUB;
            for (flags++; *flags != '\0'; flags++)
              {
                if (*flags == 'i')
                  regex_flags |= REG_ICASE;
                else
                  msg(msg_warn, COMMON_MSG_FMTN, item);
              }
            errcode = regcomp(&search->regexps_buf[search->regexps_cnt], buf, regex_flags);
            FREE(buf);
            if (errcode != 0)
              {
                buf_size = 4096;
                CALLOC(buf, buf_size, sizeof(char));
                regerror(errcode, &search->regexps_buf[search->regexps_cnt], buf, buf_size);
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_RXCOMPFAIL, buf);
                FREE(buf);
                return 1;
              }
            search->regexps_cnt += 1;
#else
            msg(msg_info, "%s.\n", MSG_S_RXDISABLED);
            msg(msg_warn, COMMON_MSG_FMTN, MSG_S_RXIGNORE, item);
            continue;
#endif
            break;
          case undef :
          default :
            msg(msg_error, COMMON_MSG_FMTN, MSG_S_UNKNTYPE, item);
            break;
        }

    }

  /* build string index */
  if (search->strings_buf.items > 0)
    {
      item = NULL;
      i = 0;
      for (i = 0; data_items_walk(&search->strings_buf, &item) > 0; i++)
        search->strings_idx[i] = item;
    }

  return 0;
}

void
search_free(search_t * const search)
{
#ifdef REGEX_SEARCH
  uint8_t i = 0;
#endif

  data_clear(&search->strings_buf);

#ifdef REGEX_SEARCH
  for (i = 0; i < search->regexps_cnt; i++)
    regfree(&search->regexps_buf[i]);
#endif

  memset(search, 0x0, sizeof(search_t));
}
