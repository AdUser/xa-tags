void
_handle_tag_add(const char *path, const char *str)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t sel_tags; /* selected tags */
  data_t all_tags; /* resulting tags set */
  data_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&sel_tags, 0, sizeof(data_t));
  memset(&all_tags, 0, sizeof(data_t));
  memset(&tmp_tags, 0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      file_uuid_set(path, &uuid);
    }

  db_tags_get(&uuid, &all_tags);

  /* this allow import existing uuid's */
  if (all_tags.items == 0)
    db_file_add(path, &uuid);

#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  data_merge(&all_tags, &tmp_tags);
#endif

  data_parse_tags(&sel_tags, str);
  data_merge(&all_tags, &sel_tags);

  db_tags_set(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  data_clear(&sel_tags);
  data_clear(&all_tags);
  data_clear(&tmp_tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  uuid_t uuid;
  data_t sel_tags; /* selected tags */
  data_t all_tags; /* resulting tags set */
  data_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&sel_tags, 0, sizeof(data_t));
  memset(&all_tags, 0, sizeof(data_t));
  memset(&tmp_tags, 0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      if (file_uuid_set(path, &uuid) > 0)
        return;
    }

  db_tags_get(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  data_merge(&all_tags, &tmp_tags);
#endif

  data_parse_tags(&sel_tags, str);

  while(data_items_walk(&sel_tags, &item) > 0)
    data_item_del(&all_tags, item);

  db_tags_set(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  data_clear(&all_tags);
  data_clear(&sel_tags);
  data_clear(&tmp_tags);
}

void
_handle_tag_clr(const char *path, const char *tags)
{
  uuid_t uuid = { 0, 0, 0 };

  if (file_uuid_get(path, &uuid) == 0)
    db_file_del(&uuid);

  file_uuid_clr(path);
#ifdef INLINE_TAGS
  file_tags_clr(path);
#endif
}

void
_handle_tag_set(const char *path, const char *str)
{
  data_t tags;
  uuid_t uuid;
  int ret = 0;

  memset(&tags, 0, sizeof(data_t));

  if (strlen(str) == 0)
    {
      _handle_tag_clr(path, NULL);
      return;
    }

  data_parse_tags(&tags, str);
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
  uuid_t uuid = { 0, 0, 0 };
  data_t all_tags; /* resulting tags set */
  data_t tmp_tags; /* temporary tags set */

  memset(&all_tags, 0x0, sizeof(data_t));
  memset(&tmp_tags, 0x0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) != 0)
    return;

  db_tags_get(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  data_merge(&all_tags, &tmp_tags);
#endif

  data_items_merge(&all_tags, ' ');
  printf("%s: %s\n", path, (all_tags.items > 0) ? all_tags.buf : "");

  data_clear(&all_tags);
  data_clear(&tmp_tags);
}

void
_handle_tag_search(const char *unused, const char *tag)
{
  char *p = NULL;
  data_t results;

  memset(&results, 0x0, sizeof(data_t));

  db_tags_find(tag, &results);

  if (results.items > 0)
    while(data_items_walk(&results, &p) > 0)
      printf("%s\n", p);
}

/* extended operations */
void
_handle_file_search_tag(const char *unused, const char *str)
{
  data_t tags;
  data_t results;

  memset(&tags,    0, sizeof(data_t));
  memset(&results, 0, sizeof(data_t));

  data_parse_tags(&tags, str);

  if (tags.items == 0)
    return;

  db_file_search_tag(&tags, &results);

  if (results.items == 0)
    return;

  data_items_merge(&results, '\n');
  printf("%s\n", results.buf);
}

void
_handle_file_search_path(const char *unused, const char *substr)
{
  data_t results;

  memset(&results, 0, sizeof(data_t));

  if (strlen(substr) == 0)
    return;

  db_file_search_path(substr, &results);

  if (results.items == 0)
    return;

  data_items_merge(&results, '\n');
  printf("%s\n", results.buf);
}

/* update operations */

/** return codes:
 * 0 - all ok
 * 1 - error
 */
int
_real_file_update(const char *path, uuid_t * uuid, struct stat *st)
{
  uuid_t uuid_linked = { 0, 0, 0 };
  data_t files;

  memset(&files, 0, sizeof(data_t));

  if (db_file_get(uuid, &files) > 0)
    return 1;

  if (files.items == 0)
    {
      msg(msg_warn, MSG_D_NOUUID, uuid_id_printf(uuid));
      return 1;
    }

  if (st->st_nlink == 1)
    {
      if (strncmp(files.buf, path, PATH_MAX) != 0)
        {
          db_file_update(path, uuid);
          msg(msg_info, MSG_F_UPDATED, path);
        }
    }

  if (st->st_nlink > 1)
    {
      if ((file_uuid_get(files.buf, &uuid_linked) == 0)
          && (uuid_linked.id == uuid->id))
        msg(msg_warn, MSG_F_LINKED, path, files.buf);
      else
        db_file_update(path, uuid);
    }

  data_clear(&files);

  return 0;
}

void
_handle_file_update(const char *path, const char *unused)
{
  struct stat st;
  uuid_t uuid = { 0, 0, 0 };

  if (stat(path, &st) != 0)
    return;

  if (file_uuid_get(path, &uuid) > 0)
    return;

  _real_file_update(path, &uuid, &st);
}

void
_handle_file_update_recursive(const char *path, const char *unused)
{
  struct stat st;
  uuid_t uuid = { 0, 0, 0 };
  char buf[PATH_MAX];
  /* fts-related variables */
  const int fts_flags = FTS_PHYSICAL | FTS_NOCHDIR;
  FTS *fts = NULL;
  FTSENT *ftsent = NULL;
  char *fts_argv[2];

  stat(path, &st);

  if (S_ISREG(st.st_mode))
    {
      if (file_uuid_get(path, &uuid) == 0)
        _real_file_update(path, &uuid, &st);
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
      if (file_uuid_get(ftsent->fts_path, &uuid) > 0)
         continue;

      if (ftsent->fts_info & FTS_F)
        _real_file_update(ftsent->fts_path, &uuid, ftsent->fts_statp);

      if (ftsent->fts_info & (FTS_DP | FTS_D))
        {
          snprintf(buf, PATH_MAX, "%s/", ftsent->fts_path);
          _real_file_update(buf, &uuid, ftsent->fts_statp);
        }
    }
  fts_close(fts);
}

int
main(int argc, char **argv)
{
  int ret = 0;
  data_t files;
  struct stat st;
  char opt = 0;
  char op = '\0';
  char *str  = NULL;
  char *item = NULL;
  char buf[PATH_MAX];
  void (*handler)(const char *, const char *) = NULL;
  bool recursive = false;

  memset(&files, 0, sizeof(data_t));

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "rvhq" "cl" "a:d:s:" "f:F:" "T:" "u")) != -1)
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
        /* options */
        case 'v' : verbosity = log_extra; break;
        case 'q' : verbosity = log_quiet; break;
        case 'r' : recursive = true; break;
        case 'h' : usage(EXIT_SUCCESS); break;
        default  : usage(EXIT_FAILURE); break;
      }

  if (op == '\0')
    usage(EXIT_FAILURE);

  if (op == 'u' && recursive == true)
    handler = &_handle_file_update_recursive;

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

          data_item_add(&files, buf, 0);
          FREE(item);
        }
      else
        printf("%s -- %s\n", argv[optind], strerror(errno));
    }

  /* init */
  opts.db.path = db_find_path_user();
  db_open();

  switch (op)
    {
      case 'f' :
      case 'F' :
#ifdef UNIQ_TAGS_LIST
      case 'T' :
#endif
        handler(NULL, str);
        break;
      case 'a' :
      case 'd' :
      case 's' :
      case 'c' :
      case 'l' :
      case 'u' :
        if (files.items < 1)
          exit(EXIT_FAILURE);
        while ((ret = data_items_walk(&files, &item)) > 0)
          handler(item, str);
        break;
      default :
        usage(EXIT_FAILURE);
    }

  data_clear(&files);

  db_close();

  exit(EXIT_SUCCESS);
}
