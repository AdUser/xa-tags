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
#include <fts.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <libgen.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <attr/xattr.h>
#include <sys/un.h>
#include <time.h>

#ifdef HAVE_GETTEXT
  #include <locale.h>
  #include <libintl.h>
#else
  #define gettext(x) (x)
#endif

#ifdef HAVE_SQLITE3
  #include <sqlite3.h>
#endif

#ifdef REGEX_SEARCH
  #include <regex.h>
#endif

#include "data.h"
#include "uuid.h"
#include "db.h"
#include "file.h"
#include "buf.h"
#include "conn.h"
#include "proto.h"
#include "search.h"
#include "log.h"
#include "msg.h"
#include "options.h"
#include "wrappers.h"

#define MAXLINE 4096

#define PROGNAME "xa-tags"
#define VERSION_MAJOR 0
#define VERSION_MINOR 3

#define XATTR_UUID  "user.xat.uuid"
#define XATTR_TAGS  "user.xat.tags"

/** hash & checksum functions */

uint16_t crc16(const char *str, size_t len);
uint32_t jhash(char *str, size_t len);
void get_path_checksums(uuid_t * const uuid, const char *path);

/** string functions */
char *normalize_path(const char *path);
#ifndef _GNU_SOURCE
void *memmem(const void *haystack, size_t haystacklen,
             const void *needle,   size_t needlelen);
#endif

void mkdir_r(const char *path, mode_t mode);

/** custom printf's */
size_t snprintf_m_uuid_file(char *buf, size_t buf_len, const uuid_t *uuid, const char *path);
size_t snprintf_m_uuid_tags(char *buf, size_t buf_len, const uuid_t *uuid, const char *tags);
