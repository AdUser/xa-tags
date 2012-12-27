void
_handle_tag_add(const char *path, const char *str)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t add_tags;
  data_t tags;

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&add_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      file_uuid_set(path, &uuid);
    }

  db_tags_get(&uuid, &tags);
#ifdef INLINE_TAGS
  file_tags_get(path, &tags);
#endif

  data_parse_tags(&add_tags, str);
  data_merge(&tags, &add_tags);

  db_tags_set(&uuid, &tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &tags);
#endif

  data_clear(&add_tags);
  data_clear(&tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  uuid_t uuid;
  data_t tags;
  data_t del_tags;

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&del_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      if (file_uuid_set(path, &uuid) > 0)
        return;
    }

  db_tags_get(&uuid, &tags);
#ifdef INLINE_TAGS
  file_tags_get(path, &tags);
#endif

  data_parse_tags(&del_tags, str);

  while(data_items_walk(&del_tags, &item) > 0)
    data_item_del(&tags, item);

  db_tags_set(&uuid, &tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &tags);
#endif

  data_clear(&del_tags);
  data_clear(&tags);
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
  data_t tags;

  memset(&tags, 0x0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) != 0)
    return;

  if (db_tags_get(&uuid, &tags) != 0)
    return;

#ifdef INLINE_TAGS
  file_tags_get(path, &tags);
#endif

  data_items_merge(&tags, ' ');
  printf("%s: %s\n", path, (tags.len > 0) ? tags.buf : "");

  data_clear(&tags);
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

  /* TODO: unimplemented */
  if (tags.items >= 2)
    msg(msg_warn, "Search by multiple tags unimplemented, only first will be used.\n");

  db_file_search_tag(tags.buf, &results);

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

  stat(files.buf, st);

  if (st->st_nlink == 1)
    if (strncmp(files.buf, path, PATH_MAX) != 0)
      {
        db_file_update(path, uuid);
        msg(msg_info, MSG_F_UPDATED, path);
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

  if (file_uuid_get(path, &uuid) > 0)
    return;

  _real_file_update(path, &uuid, &st);
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

  memset(&files, 0, sizeof(data_t));

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "vhq" "cl" "a:d:s:" "f:F:" "T:" "u")) != -1)
    {
      op = opt;
      str = optarg;
      switch (opt)
        {
          case 'a' : handler = &_handle_tag_add; break;
          case 'd' : handler = &_handle_tag_del; break;
          case 's' : handler = &_handle_tag_set; break;
          case 'c' : handler = &_handle_tag_clr; break;
          case 'l' : handler = &_handle_tag_lst; break;
          case 'f' : handler = &_handle_file_search_tag;  break;
          case 'F' : handler = &_handle_file_search_path; break;
          case 'T' : handler = &_handle_tag_search; break;
          case 'u' : handler = &_handle_file_update; break;
          case 'v' : verbosity = log_extra; break;
          case 'q' : verbosity = log_quiet; break;
          case 'h' : usage(EXIT_SUCCESS); break;
          default  : usage(EXIT_FAILURE); break;
        }
    }

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
          usage(EXIT_FAILURE);
        while ((ret = data_items_walk(&files, &item)) > 0)
          handler(item, str);
        break;
    }

  data_clear(&files);

  db_close();

  exit(EXIT_SUCCESS);
}
