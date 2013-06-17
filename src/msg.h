/*  no trailing '\n', please! */

/* used in fatal errors, expects (__FILE__, __LINE__, custom message) */
#define COMMON_AST_FMT "%s:%i -- %s\n"
/* used in data validation, expects (error string, bad data) */
#define COMMON_ERR_FMT  "%s -- %s"
#define COMMON_ERR_FMTN "%s -- %s\n"
#define COMMON_MSG_FMT  "%s: %s"
#define COMMON_MSG_FMTN "%s: %s\n"

/** log messages: FILE:LINE -- message  *
  * usable with ASSERT() macros         */
#define MSG_M_OOM        gettext("Can't allocate memory.")
#define MSG_M_NULLPTR    gettext("Unexpected NULL pointer.")
#define MSG_M_REALLOC    gettext("realloc() failed.")

/** IPC messages */

#define MSG_I_HELLO      \
  gettext("glad to meet you.")
#define MSG_I_BYE        \
  gettext("Sya!")

/** parsing errors */
/* trailing dot! */
#define MSG_I_BADREQ     \
  gettext("Parsing failed, unknown request type.")
#define MSG_I_BADOP      \
  gettext("Parsing failed, unknown operation for this type.")
#define MSG_I_EXPREQ     \
  gettext("Parsing failed, request expected.")
#define MSG_I_EXPOP      \
  gettext("Parsing failed, operation expected.")
#define MSG_I_EXPDELIM   \
  gettext("Parsing failed, expected ':', '>' char.")
#define MSG_I_EXPDATA    \
  gettext("Parsing failed, data expected.")

/** data validation errors */
/* no trailing dot also, please */
#define MSG_I_BADPATH    \
  gettext("Malformed path")
#define MSG_I_BADUUID    \
  gettext("Malformed UUID")
#define MSG_I_BADCHARS   \
  gettext("Unallowed chars")
#define MSG_I_EXPPATH    \
  gettext("Empty data, expected path")
#define MSG_I_EXPTAGS    \
  gettext("Empty data, expected one or more tags")

/** request processing errors */
#define MSG_I_NOBATCH    \
  gettext("No batch requests allowed.")
#define MSG_I_PARTREQ    \
  gettext("Request processed partly.")
#define MSG_I_ABORTREQ   \
  gettext("Request discarded.")

#define MSG_I_DBRDONLY   \
  gettext("Database opened readonly. Request aborted.")

/** search **/
#define MSG_S_MAXREACHED \
  gettext("Maximum search conditions reached, ignoring")
#define MSG_S_UNKNTYPE \
  gettext("Undefined type for the search term")
#define MSG_S_SHORTTERM \
  gettext("Search term too short, ignored")
#define MSG_S_UNKNRXFLAG \
  gettext("Unknown flag in this regex")
#define MSG_S_RXCOMPFAIL \
  gettext("Regex compilation error")
#define MSG_S_RXDISABLED \
  gettext("Regex support disabled, ignoring")
#define MSG_S_RXONLY \
  gettext("Search only with regexps may be extremely slow!")
#define MSG_S_EMPTY \
  gettext("Empty search leads to empty result")

/** connection errors */
#define MSG_I_TIMEOUT    \
  gettext("Connection timeout.")

/** other errors */
#define MSG_U_UNKNERR    \
  gettext("Internal error occured")

#define MSG_U_HOMEUNSET  \
  gettext("$HOME variable unset, don't know where to look for database.")

/* file handle messages */
/* no trailing dot also, please */
#define MSG_F_UPDATED    \
  gettext("File updated")
#define MSG_F_LINKED     \
  gettext("Hardlinked file, ignored")
#define MSG_F_FAILOPEN   \
  gettext("Can't open")
#define MSG_F_NOTSUPP    \
  gettext("xattrs disabled or not supported")

/** database errors */
/* no trailing dot also, please */
#define MSG_D_DBCREATED \
  gettext("Created new database")
#define MSG_D_LOADED \
  gettext("Loaded database")
#define MSG_D_FAILOPEN \
  gettext("Failed to open database")
#define MSG_D_FAILCLOSE \
  gettext("Failed to close database")
#define MSG_D_FAILCREATE \
  gettext("Failed to init database")
#define MSG_D_FAILPREPARE \
  gettext("Failed to prepare statement")
#define MSG_D_FAILEXEC \
  gettext("Failed to exec statement")
#define MSG_D_BADINPUT \
  gettext("Malformed or incorrect data on input")
#define MSG_D_ZEROUUID \
  gettext("Successfull insert, but zero uuid returned")
#define MSG_D_NOUUID     \
  gettext("No such uuid")
#define MSG_D_NOVERSION \
  gettext("failed to determine db version")
#define MSG_D_VERMISMATCH \
  gettext("Database version mismatch. Expected version")
