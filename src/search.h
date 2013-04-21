/** typedefs */

#define MAX_SEARCH_COND 32

#define SEARCH_NONE   0 << 0
#define SEARCH_NEG    1 << 0
#define SEARCH_EXACT  1 << 1

typedef struct search_t
{
  uint8_t   str_flags[MAX_SEARCH_COND];
  data_t    str_buf;
#ifdef REGEX_SEARCH
  uint8_t   rxp_cnt;
  uint8_t   rxp_flags[MAX_SEARCH_COND];
  regex_t   rxp_buf[MAX_SEARCH_COND];
#endif
} search_t;

/** functions */

/** return value:
 * 0 - on success
 * 1 - on error
 */
int
search_parse_terms(search_t * const search, const data_t *terms);

/**
 * gracefully free all allocated memory
 */
void
search_free(search_t * const search);
