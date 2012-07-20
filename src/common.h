#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_GETTEXT
  #include <libintl.h>
  #define _(x) gettext((x))
#else
  #define _(x) (x)
#endif

#include "data.h"
#include "conn.h"
#include "proto.h"
#include "log.h"
#include "msg.h"
#include "options.h"
#include "wrappers.h"

#define MAXLINE 4096

