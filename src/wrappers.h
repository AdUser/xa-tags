#define CALLOC(ptr, nmemb, size) \
  if (((ptr) = calloc((nmemb), (size))) == NULL) \
    msg(msg_error, MSG_M_OOM, __FILE__, __LINE__)
    /* yes, i know about assert() */

#define FREE(ptr) \
  free((ptr)); \
  (ptr) = NULL

#define STRNDUP(ptr, str, len) \
  if (((ptr) = strndup((str), (len))) == NULL) \
    msg(msg_error, MSG_M_OOM, __FILE__, __LINE__)

