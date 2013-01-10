/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "common.h"

#define  DUMB_TOOL   1
#define SMART_TOOL   2
#define  DUMB_CLIENT 3
#define SMART_CLIENT 4

#define F_RECURSE    1 << 0
#define F_KEEPCONV   1 << 1

uint8_t flags = 0;
enum loglevel verbosity = log_normal;

void
usage(int exitcode)
{
  fprintf(stdout,
  "%s  version %u.%u\n",
  PROGNAME, VERSION_MAJOR, VERSION_MINOR);

  fprintf(stdout,
  "Usage: %s [options] <file> [<file>]\n"
  "\n", PROGNAME);

  #include "client/usage_common.inc.c"
  #include "client/usage_basic.inc.c"
#if MODE >= SMART_TOOL
  #include "client/usage_search.inc.c"
  #include "client/usage_update.inc.c"
#endif

  fprintf(stdout, "\n");
  exit(exitcode);
}

void
_ftw(const char *path, const char *str, void (*handler)(const char *, const char *))
{
  struct stat st;
  /* fts-related variables */
  const int fts_flags = FTS_PHYSICAL | FTS_NOCHDIR;
  char *fts_argv[2];
  FTS *fts = NULL;
  FTSENT *ftsent = NULL;

  stat(path, &st);

  if (S_ISREG(st.st_mode))
    {
      handler(path, str);
      return;
    }

  if (!S_ISDIR(st.st_mode))
    return;

  fts_argv[0] = (char * const) path;
  fts_argv[1] = NULL;

  if ((fts = fts_open(fts_argv, fts_flags, NULL)) == NULL)
    msg(msg_error, MSG_F_FAILOPEN, path);

  while ((ftsent = fts_read(fts)) != NULL)
    {
      if (ftsent->fts_info & FTS_F)
        handler(ftsent->fts_path, str);

      if (ftsent->fts_info & (FTS_DP | FTS_D))
        handler(ftsent->fts_path, str);
    }

  fts_close(fts);
}

#if   MODE ==  DUMB_TOOL
  #include "client/dumb_tool.inc.c"
#elif MODE == SMART_TOOL
  #include "client/smart_tool.inc.c"
#elif MODE ==  DUMB_CLIENT
#elif MODE == SMART_CLIENT
#else
  /* do nothing */
#endif
