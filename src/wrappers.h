#ifdef DEBUG
  #define ASSERT(x, y) \
    if (!(x)) msg(msg_error, COMMON_AST_FMT, __FILE__, __LINE__, (y))
#else
  #define ASSERT(x, y) (x)
#endif

/**
 @brief  safely frees allocated string
 */
#define FREE(ptr) \
  if (ptr != NULL) free((ptr)); \
  (ptr) = NULL

/**
 @brief  safely allocates a lot of memory
 */
#define CALLOC(ptr, nmemb, size) \
  ASSERT((ptr) = calloc((nmemb), (size)), MSG_M_OOM)

/**
 @brief  safely changes size of allocated memory
 */
#define REALLOC(new_ptr, old_ptr, new_size) \
  ASSERT((new_ptr) = realloc((old_ptr), (new_size)), MSG_M_REALLOC)

#define STRNDUP(ptr, str, len) \
  ASSERT((((ptr) = strndup((str), (len))) != NULL), MSG_M_OOM)

