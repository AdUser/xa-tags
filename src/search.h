/** typedefs */

#define MAX_SEARCH_COND 32

typedef struct search_t
{
  list_t    substr;
  list_t    exact;
#ifdef REGEX_SEARCH
  uint8_t   regex_cnt;
  bool      regex_neg[MAX_SEARCH_COND];
  regex_t   regexps[MAX_SEARCH_COND];
#endif
} search_t;

/** functions */

/** return value:
 * 0 - on success
 * 1 - on error
 */
int
search_parse_terms(search_t * const search, const list_t *terms);

/**
 * gracefully free all allocated memory
 */
void
search_free(search_t * const search);

/** return value:
 * -1 - error
 *  0 - not match
 *  1 - match
 */
int
search_match_substr(const search_t *search, const char *tags);

/** return value:
 * -1 - error
 *  0 - not match
 *  1 - match
 */
int
search_match_exact(const search_t *search, const char *tags);

#ifdef REGEX_SEARCH
/** return value:
 * -1 - error
 *  0 - not match
 *  1 - match
 */
int
search_match_regex(const search_t *search, const char *tags);
#endif

/* top level function, combines substring, exact and regex match */
/** return value:
 *  0 - not match
 *  1 - match
 */
int
search_match(const search_t * search, const char *tags);
