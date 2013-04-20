/* typedefs */

#define MAX_SEARCH_COND 32

typedef struct search_t
{
  uint8_t  strings_cnt;
  bool     strings_neg[MAX_SEARCH_COND];
  char    *strings[MAX_SEARCH_COND];
  char    *strings_buf;
#ifdef REGEX_SEARCH
  uint8_t   regexps_cnt;
  bool      regexps_neg[MAX_SEARCH_COND];
  regex_t (*regexps)[MAX_SEARCH_COND];
#endif
} search_t;

/* functions */

/** return value:
 * 0 - on success
 * 1 - on error
 */
int
parse_search_tags(search_t * const search, const data_t *conds);
