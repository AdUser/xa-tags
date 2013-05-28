/** typedefs */

#define MAX_SEARCH_COND 32

/**
 @struct search_t
 @brief  Type for store information for filtering search results
 */
typedef struct search_t
{
  list_t    substr;    /**< token(s) to match against all string with tags */
  list_t    exact;     /**< token(s) to match against each tag */
#ifdef REGEX_SEARCH
  uint8_t   regex_cnt; /**< regexps count */
  bool      regex_neg[MAX_SEARCH_COND];  /**< negate flag for regexp */
  regex_t   regexps[MAX_SEARCH_COND];    /**< compiled regexps cache */
#endif
} search_t;

/** functions */

int  search_parse_terms(search_t * const search, const list_t *terms);
void search_free(search_t * const search);

int  search_match_substr(const search_t *search, const char *tags);
int  search_match_exact(const search_t *search, const char *tags);
#ifdef REGEX_SEARCH
int  search_match_regex(const search_t *search, const char *tags);
#endif

/* top level function, combines substring, exact and regex match */
int  search_match(const search_t * search, const char *tags);
