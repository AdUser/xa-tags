#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef HAVE_GETTEXT
  #include <libintl.h>
  #define _(x) gettext((x))
#else
  #define _(x) (x)
#endif

#include "data.h"
#include "uuid.h"
#include "buf.h"
#include "conn.h"
#include "proto.h"
#include "log.h"
#include "msg.h"
#include "options.h"
#include "wrappers.h"

#define MAXLINE 4096

#define PROGNAME "xa-tags"
#define VERSION_MAJOR 0
#define VERSION_MINOR 1

/** hash & checksum functions */

uint16_t crc16(char *str, size_t len);
uint32_t jhash(char *str, size_t len);
