#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <yaml.h>

#ifdef HAVE_GETTEXT
  #include <libintl.h>
  #define _(x) gettext((x))
#else
  #define _(x) (x)
#endif

#include "log.h"
#include "msg.h"
#include "options.h"
#include "proto.h"
#include "wrappers.h"

#define MAXLINE 4096

