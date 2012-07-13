/** log messages: FILE:LINE -- message  *
  * usable with ASSERT() macros         */
#define MSG_M_OOM        _("%s:%i -- Can't allocate memory.\n")
#define MSG_M_NULLPTR    _("%s:%i -- Unexpected NULL pointer.\n")
#define MSG_M_REALLOC    _("%s:%i -- realoc() failed.\n")

/** IPC messages */
#define MSG_I_BADREQ     \
  _("Parsing failed, unknown request type.\n")
#define MSG_I_BADOP      \
  _("Parsing failed, unknown operation for this type.\n")
#define MSG_I_BADMARKER  \
  _("Parsing failed, expected ':', '>' or '\\n' char.\n")
#define MSG_I_EXPOP      \
  _("Parsing failed, operation expected.\n")
#define MSG_I_EXPDATA    \
  _("Parsing failed, expected data.\n")
#define MSG_I_TIMEOUT    \
  _("Connection timeout.\n")
#define MSG_I_UNKNERR    \
  _("Internal error occured. Request aborted.\n")
