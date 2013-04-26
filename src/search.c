#include "common.h"

int
search_parse_terms(search_t * const search, const list_t *terms)
{
  char *item = NULL;
  char *flags = NULL;
  uint8_t term_flags = 0;
  size_t item_len = 0;
  list_t strings;
  enum { undef, string, regexp } type;
#ifdef REGEX_SEARCH
  char *buf = NULL;
  size_t buf_size = 4096;
  int regex_flags;
  int errcode = 0;
#endif

  memset(&strings, 0x0, sizeof(list_t));

  while (list_items_walk(terms, &item) > 0)
    {
      item_len = strlen(item);

      /* flags */
      term_flags = 0;
      if (*item == '-' || *item == '=')
        {
          term_flags = (*item == '-') ? SEARCH_NEG : SEARCH_EXACT;
          item++;
          item_len--;
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
            if (search->str_buf.items >= MAX_SEARCH_COND)
              {
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            search->str_flags[search->str_buf.items] = term_flags;
            list_item_add(&search->str_buf, item, item_len);
            break;
          case regexp :
#ifdef REGEX_SEARCH
            if (search->rxp_cnt >= MAX_SEARCH_COND)
              {
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_MAXREACHED, item);
                continue;
              }
            search->rxp_flags[search->rxp_cnt] = term_flags & ~(SEARCH_EXACT);
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
            errcode = regcomp(&search->rxp_buf[search->rxp_cnt], buf, regex_flags);
            FREE(buf);
            if (errcode != 0)
              {
                buf_size = 4096;
                CALLOC(buf, buf_size, sizeof(char));
                regerror(errcode, &search->rxp_buf[search->rxp_cnt], buf, buf_size);
                msg(msg_warn, COMMON_MSG_FMTN, MSG_S_RXCOMPFAIL, buf);
                FREE(buf);
                return 1;
              }
            search->rxp_cnt += 1;
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

  return 0;
}

void
search_free(search_t * const search)
{
#ifdef REGEX_SEARCH
  uint8_t i = 0;
#endif

  list_clear(&search->str_buf);

#ifdef REGEX_SEARCH
  for (i = 0; i < search->rxp_cnt; i++)
    regfree(&search->rxp_buf[i]);
#endif

  memset(search, 0x0, sizeof(search_t));
}
