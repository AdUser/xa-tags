/** log messages: FILE:LINE -- message  *
  * usable with ASSERT() macros         */
#define MSG_M_OOM        _("%s:%i -- Can't allocate memory.\n")
#define MSG_M_NULLPTR    _("%s:%i -- Unexpected NULL pointer.\n")
#define MSG_M_REALLOC    _("%s:%i -- realoc() failed.\n")

/** IPC messages */
/*  no trailing '\n', please */

/** parsing errors */
#define MSG_I_BADREQ     \
  _("Parsing failed, unknown request type.")
#define MSG_I_BADOP      \
  _("Parsing failed, unknown operation for this type.")
#define MSG_I_EXPREQ     \
  _("Parsing failed, request expected.")
#define MSG_I_EXPOP      \
  _("Parsing failed, operation expected.")
#define MSG_I_EXPDELIM   \
  _("Parsing failed, expected ':', '>' char.")
#define MSG_I_EXPDATA    \
  _("Parsing failed, data expected.")

/** data validation errors */
#define MSG_I_BADPATH    \
  _("Malformed path")
#define MSG_I_BADUUID    \
  _("Malformed UUID")
#define MSG_I_BADCHARS   \
  _("Unallowed chars")
#define MSG_I_EXPPATH    \
  _("Empty data, expected path")
#define MSG_I_EXPTAGS    \
  _("Empty data, expected one or more tags")

/** request processing errors */
#define MSG_I_NOBATCH    \
  _("No batch reqests allowed.")
#define MSG_I_PARTREQ    \
  _("Request processed partly.")
#define MSG_I_ABORTREQ   \
  _("Request discarded.")

/** connection errors */
#define MSG_I_TIMEOUT    \
  _("Connection timeout.")

/** database errors */
#define MSG_I_DBRDONLY   \
  _("Database readonly.")

/** other errors */
#define MSG_I_UNKNERR    \
  _("Internal error occured. Request aborted.")
