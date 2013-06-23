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

#include "main.h"

/* global variables */
search_t *search = NULL;

void
usage(int exitcode)
{
  fprintf(stdout,
  "%s  version %u.%u\n",
  PROGNAME, VERSION_MAJOR, VERSION_MINOR);

  fprintf(stdout,
  "Usage: %s [options] <file> [<file>]\n"
  "\n", PROGNAME);

#include "usage/common.c.inc"
#include "usage/basic.c.inc"
#include "usage/search.c.inc"
#include "usage/update.c.inc"
#include "usage/db_misc.c.inc"
  fprintf(stdout, "\n");

#ifndef INLINE_TAGS
#include "usage/convert_storage.c.inc"
  fprintf(stdout, "\n");
#endif

  exit(exitcode);
}

/* handlers */
void
_handle_tag_add(const char *path, const char *str)
{
  uuid_t uuid = 0;
  list_t all_tags; /* resulting tags set */
  list_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&all_tags, 0, sizeof(list_t));
  memset(&tmp_tags, 0, sizeof(list_t));

  file_uuid_get(path, &uuid);

  if (uuid != 0)
    db_tags_get(&uuid, &all_tags);

  if (uuid == 0)
    {
      if (db_file_add(path, &uuid) > 0 ||
          file_uuid_set(path, &uuid) > 0)
        return;
    }

#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  list_merge(&all_tags, &tmp_tags);
#endif

  list_parse_tags(&tmp_tags, str);
  list_merge(&all_tags, &tmp_tags);

  db_tags_set(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  list_clear(&all_tags);
  list_clear(&tmp_tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  uuid_t uuid;
  bool save = false;
  list_t all_tags; /* resulting tags set */
  list_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&all_tags, 0, sizeof(list_t));
  memset(&tmp_tags, 0, sizeof(list_t));

  if (file_uuid_get(path, &uuid) > 0)
    return;

  db_tags_get(&uuid, &all_tags);

  if (all_tags.items == 0)
    return; /* nothing to delete */

#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  list_merge(&all_tags, &tmp_tags);
#endif

  list_parse_tags(&tmp_tags, str);

  while(list_items_walk(&tmp_tags, &item) > 0)
    if (list_item_del(&all_tags, item) == 1)
      save = true;

  if (save == true)
    db_tags_set(&uuid, &all_tags);

#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  list_clear(&all_tags);
  list_clear(&tmp_tags);
}

int
_cb_tag_clear(const char *unused, const uuid_t *uuid)
{
  return db_file_del(uuid);
}

void
_handle_tag_clr(const char *path, const char *tags)
{
  struct stat st;
  query_limits_t lim = { 0, 250 };
  uuid_t uuid = 0;

  if (stat(path, &st) == 0 && file_uuid_get(path, &uuid) == 0)
    {
      db_file_del(&uuid);
      db_tags_clr(&uuid);
#ifdef INLINE_TAGS
      file_tags_clr(path);
#endif
      return;
    }

  db_file_search_path(path, &lim, NULL, _cb_tag_clear);
}

void
_handle_tag_set(const char *path, const char *str)
{
  list_t tags;
  uuid_t uuid = 0;
  int ret = 0;

  memset(&tags, 0, sizeof(list_t));

  if (strlen(str) == 0)
    {
      _handle_tag_clr(path, NULL);
      return;
    }

  list_parse_tags(&tags, str);
  if ((ret = file_uuid_get(path, &uuid)) == 2)
    return;

  if (ret == 1)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      file_uuid_set(path, &uuid);
    }

  db_tags_set(&uuid, &tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &tags);
#endif
}

void
_handle_tag_lst(const char *path, const char *unused)
{
  uuid_t uuid = 0;
  list_t all_tags; /* resulting tags set */
  list_t tmp_tags; /* temporary tags set */

  memset(&all_tags, 0x0, sizeof(list_t));
  memset(&tmp_tags, 0x0, sizeof(list_t));

  if (file_uuid_get(path, &uuid) != 0)
    return;

  db_tags_get(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  list_merge(&all_tags, &tmp_tags);
#endif

  list_items_merge(&all_tags, ' ');

  if (all_tags.items > 0 || verbosity > log_normal)
    printf(COMMON_MSG_FMTN, path, (all_tags.items > 0) ? all_tags.buf : "");

  list_clear(&all_tags);
  list_clear(&tmp_tags);
}

#ifdef UNIQ_TAGS_LIST
void
_handle_tag_search(const char *unused, const char *tag)
{
  char *p = NULL;
  list_t results;
  query_limits_t lim = { 0, MAX_QUERY_LIMIT };

  memset(&results, 0x0, sizeof(list_t));

  while (db_tags_find(tag, &lim, &results) < 2)
    {
      if (results.items == 0)
        break;

      while(list_items_walk(&results, &p) > 0)
        printf("%s\n", p);
    }

  list_clear(&results);
}
#endif

/* extended operations */
void
_search_init(const char *str)
{
  list_t terms;

  memset(&terms, 0x0, sizeof(list_t));

  list_parse_tags(&terms, str);
  CALLOC(search, 1, sizeof(search_t));
  search_parse_terms(search, &terms);
  list_clear(&terms);

  if (search->substr.items == 0 && search->exact.items == 0)
    {
#ifdef REGEX_SEARCH
      if (search->regex_cnt > 0)
        msg(msg_warn, "%s\n", MSG_S_RXONLY);
      else
        msg(msg_error, "%s\n", MSG_S_EMPTY);
#else
      msg(msg_error, "%s\n", MSG_S_EMPTY);
#endif
    }
}

void
_handle_file_search_tag(const char *unused, const char *str)
{
  int ret = 0;
  list_t results;
  query_limits_t lim = { 0, MAX_QUERY_LIMIT };

  memset(&results, 0, sizeof(list_t));

  if (search == NULL)
    _search_init(str);

  while ((ret = db_file_search_tag(search, &lim, &results, NULL)) < 2)
    {
      if (results.items == 0)
        break;

      list_items_merge(&results, '\n');
      printf("%s\n", results.buf);

      if (ret == 0)
        break;
    }

  list_clear(&results);
}

void
_handle_file_search_path(const char *unused, const char *substr)
{
  list_t results;
  query_limits_t lim = { 0, MAX_QUERY_LIMIT };
  char *buf;
  size_t len = strlen(substr);

  memset(&results, 0, sizeof(list_t));
  CALLOC(buf, len + 4, sizeof(char));

  snprintf(buf, len + 3, "%c%s%c", '*', substr, '*');

  if (strlen(substr) == 0)
    return;

  while (db_file_search_path(buf, &lim, &results, NULL) < 2)
    {
      if (results.items == 0)
        break;

      list_items_merge(&results, '\n');
      printf("%s\n", results.buf);
    }

  list_clear(&results);
  FREE(buf);
}

/* update operations */
void
_handle_labels_restore(const char *path, const char *unused)
{
  query_limits_t lim = { 0, MAX_QUERY_LIMIT };
  int (*cb)(const char *, const uuid_t *) = &file_uuid_set;
  char *buf = NULL;
  char c = '\0';
  size_t len = strlen(path);
  struct stat st;

  if (len == 0)
    return;

  stat(path, &st);

  CALLOC(buf, len + 2, sizeof(char));

  if (S_ISDIR(st.st_mode) && (flags & F_RECURSE))
    c = '*';

  snprintf(buf, len + 2, "%s%c", path, c);

  while (db_file_search_path(buf, &lim, NULL, cb) == 1);

  FREE(buf);
}

void
_handle_file_update(const char *path, const char *str)
{
  struct stat st;
  uuid_t uuid = 0;
  uuid_t uuid_linked = 0;
  char buf[PATH_MAX] = { 0 };

  if (stat(path, &st) != 0)
    return;

  if (file_uuid_get(path, &uuid) > 0)
    return;

  if (db_file_get(&uuid, buf) > 0)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_NOUUID, uuid_id_printf(&uuid));
      return;
    }

  if (st.st_nlink == 1 || S_ISDIR(st.st_mode))
    if (strncmp(buf, path, PATH_MAX) != 0)
      {
        db_file_update(path, &uuid);
        msg(msg_info, COMMON_MSG_FMTN, MSG_F_UPDATED, path);
      }

  if (st.st_nlink > 1)
    {
      if ((file_uuid_get(buf, &uuid_linked) == 0)
          && (uuid_linked == uuid))
        msg(msg_warn, COMMON_MSG_FMTN, MSG_F_LINKED, path);
      else
        db_file_update(path, &uuid);
    }
}

/* dirlist */
int
_dirlist_cb(uuid_t uuid, const char *path, const char *tags)
{
  if (verbosity >= log_extra)
    printf("%s: %s\n", path, tags);
  else
    puts(path);

  return 0;
}

void
_handle_file_dirlist(const char *path, const char *str)
{
  char *buf = NULL;
  query_limits_t lim = { 0, 512 };
  size_t len = 0;

  len = strlen(path);

  if (path[len] == '/')
    len--;

  CALLOC(buf, len + 1, sizeof(char));
  snprintf(buf, len, "%s", path);
  while (db_file_dirlist(buf, &lim, NULL, &_dirlist_cb) == 1);
  FREE(buf);
}

void
_handle_db_misc(const char *unused, const char *operation)
{
  if (operation == NULL)
    return;

  if (strcmp(operation, "rehash") == 0)
    db_rehash();
}

/* #ifndef INLINE_TAGS */
void
_handle_file_migrate_to_db(const char *path, const char *unused)
{
  uuid_t uuid = 0;
  list_t tags;
  int ret = 0;

  memset(&tags, 0x0, sizeof(list_t));

  if (file_tags_get(path, &tags) > 0)
    return;

  if (db_file_add(path, &uuid) > 0)
    return;

  ret = db_tags_set(&uuid, &tags);

  list_clear(&tags);

  if (ret == 0 && !(flags & F_KEEPCONV))
    file_tags_clr(path);
}

void
_handle_file_migrate_from_db(const char *path, const char *unused)
{
  uuid_t uuid = 0;
  list_t tags;
  int ret = 0;

  memset(&tags, 0x0, sizeof(list_t));

  if (file_uuid_get(path, &uuid) > 0)
    return;

  if (db_tags_get(&uuid, &tags) > 0)
    return;

  ret = file_tags_set(path, &tags);

  list_clear(&tags);

  if (ret == 0 && !(flags & F_KEEPCONV))
    db_file_del(&uuid);
}
/* #endif */

int
main(int argc, char **argv)
{
  int ret = 0;
  list_t files;
  struct stat st;
  char opt = 0;
  char op = '\0';
  char *str  = NULL;
  char *item = NULL;
  char buf[PATH_MAX];
  void (*handler)(const char *, const char *) = NULL;

  memset(&opts, 0x0, sizeof(opts));
  memset(&files, 0, sizeof(list_t));

#ifdef HAVE_GETTEXT
  setlocale (LC_ALL, "");
  bindtextdomain(PROGNAME, LOCALEDIR);
  textdomain(PROGNAME);
#endif

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "rvhqb:" "cl" "a:d:s:" "f:F:" "T:" "RuLQ:" "mMk")) != -1)
    switch (opt)
      {
        /* operations */
        case 'a' : op = opt; str = optarg; handler = &_handle_tag_add; break;
        case 'd' : op = opt; str = optarg; handler = &_handle_tag_del; break;
        case 's' : op = opt; str = optarg; handler = &_handle_tag_set; break;
        case 'c' : op = opt; str = optarg; handler = &_handle_tag_clr; break;
        case 'l' : op = opt; str = optarg; handler = &_handle_tag_lst; break;
#ifdef UNIQ_TAGS_LIST
        case 'T' : op = opt; str = optarg; handler = &_handle_tag_search; break;
#endif
        case 'f' : op = opt; str = optarg; handler = &_handle_file_search_tag;  break;
        case 'F' : op = opt; str = optarg; handler = &_handle_file_search_path; break;
        case 'u' : op = opt; str = optarg; handler = &_handle_file_update; break;
        case 'L' : op = opt; str = optarg; handler = &_handle_file_dirlist; break;
        case 'R' : op = opt; str = optarg; handler = &_handle_labels_restore; break;
        case 'Q' : op = opt; str = optarg; handler = &_handle_db_misc; break;
#ifndef INLINE_TAGS
        case 'm' : op = opt; str = NULL;   handler = &_handle_file_migrate_to_db; break;
        case 'M' : op = opt; str = NULL;   handler = &_handle_file_migrate_from_db; break;
        case 'k' : flags |= F_KEEPCONV; break;
#endif
        /* options */
        case 'v' : verbosity = log_extra; break;
        case 'q' : verbosity = log_quiet; break;
        case 'r' : flags |= F_RECURSE; break;
        case 'b' : opts.db.path = optarg; break;
        case 'h' : usage(EXIT_SUCCESS); break;
        default  : usage(EXIT_FAILURE); break;
      }

  if (op == '\0')
    usage(EXIT_FAILURE);

  for (; optind < argc; optind++)
    {
      errno = 0;
      if (stat(argv[optind], &st) == 0)
        {
          item = realpath(argv[optind], NULL);
          ret = strlen(item);

          if (ret > 0) ret--;

          if (S_ISDIR(st.st_mode))
            snprintf(buf, PATH_MAX, "%s%c", item, (item[ret] != '/') ? '/' : '\0');
          else
            snprintf(buf, PATH_MAX, "%s", item);

          list_item_add(&files, buf, 0);
          FREE(item);
        }
      else if (op == 'c' || op == 'L')
        {
          list_item_add(&files, argv[optind], 0);
        }
      else
        printf(COMMON_ERR_FMTN, argv[optind], strerror(errno));
    }

  /* init */
  if (opts.db.path == NULL)
    opts.db.path = db_find_path_user();
  db_open();

  switch (op)
    {
      case 'f' :
      case 'F' :
#ifdef UNIQ_TAGS_LIST
      case 'T' :
#endif
      case 'Q' :
        handler(NULL, str);
        break;
      case 'a' :
      case 'd' :
      case 's' :
      case 'c' :
      case 'l' :
      case 'u' :
      case 'L' :
#ifndef INLINE_TAGS
      case 'm' :
      case 'M' :
#endif
        if (files.items < 1)
          exit(EXIT_FAILURE);
        while ((ret = list_items_walk(&files, &item)) > 0)
          (flags & F_RECURSE) ? ftw_cb(item, str, handler) : handler(item, str);
        break;
      case 'R' :
        if (files.items < 1)
          exit(EXIT_FAILURE);
        while ((ret = list_items_walk(&files, &item)) > 0)
          handler(item, NULL); /* without recursion */
        break;
      default :
        usage(EXIT_FAILURE);
    }

  list_clear(&files);
  FREE(search);

  db_close();

  exit(EXIT_SUCCESS);
}
