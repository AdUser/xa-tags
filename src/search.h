/** typedefs */

#define MAX_SEARCH_COND 32

typedef struct search_t
{
  bool     strings_neg[MAX_SEARCH_COND];
  char    *strings_idx[MAX_SEARCH_COND];
  data_t   strings_buf;
#ifdef REGEX_SEARCH
  uint8_t   regexps_cnt;
  bool      regexps_neg[MAX_SEARCH_COND];
  regex_t   regexps_buf[MAX_SEARCH_COND];
#endif
} search_t;

/** functions */

/** return value:
 * 0 - on success
 * 1 - on error
 */
int
parse_search_terms(search_t * const search, const data_t *terms);

/**
 * gracefully free all allocated memory
 */
void search_free(search_t * const search);
