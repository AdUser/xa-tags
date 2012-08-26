#define CALLOC(ptr, nmemb, size) \
  if (((ptr) = calloc((nmemb), (size))) == NULL) \
    msg(msg_error, MSG_M_OOM, __FILE__, __LINE__)
    /* yes, i know about assert() */

#define REALLOC(new_ptr, old_ptr, new_size) \
  if (((new_ptr) = realloc((old_ptr), (new_size))) == NULL) \
    msg(msg_error, MSG_M_REALLOC, __FILE__, __LINE__)

#define FREE(ptr) \
  if (ptr != NULL) free((ptr)); \
  (ptr) = NULL

#define STRNDUP(ptr, str, len) \
  ASSERT((((ptr) = strndup((str), (len))) != NULL), MSG_M_OOM)

#ifdef DEBUG
  #define ASSERT(x, y) \
    if (!(x)) msg(msg_error, (y), __FILE__, __LINE__)
#else
  #define ASSERT(x, y) (x)
#endif
