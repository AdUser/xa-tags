void
_handle_tag_add(const char *path, const char *str)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t all_tags; /* resulting tags set */
  data_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

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
  if (all_tags.items == 0 && uuid.id != 0)
    db_file_add(path, &uuid);

#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  data_merge(&all_tags, &tmp_tags);
#endif

  data_parse_tags(&tmp_tags, str);
  data_merge(&all_tags, &tmp_tags);

  db_tags_set(&uuid, &all_tags);
#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  data_clear(&all_tags);
  data_clear(&tmp_tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  uuid_t uuid;
  bool save = false;
  data_t all_tags; /* resulting tags set */
  data_t tmp_tags; /* temporary tags set */

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&all_tags, 0, sizeof(data_t));
  memset(&tmp_tags, 0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    return;

  db_tags_get(&uuid, &all_tags);

  if (all_tags.items == 0)
    return; /* nothing to delete */

#ifdef INLINE_TAGS
  file_tags_get(path, &tmp_tags);
  data_merge(&all_tags, &tmp_tags);
#endif

  data_parse_tags(&tmp_tags, str);

  while(data_items_walk(&tmp_tags, &item) > 0)
    if (data_item_del(&all_tags, item) == 1)
      save = true;

  if (save == true && all_tags.items > 0)
    db_tags_set(&uuid, &all_tags);

  if (save == true && all_tags.items == 0)
    db_file_del(&uuid);

#ifdef INLINE_TAGS
  file_tags_set(path, &all_tags);
#endif

  data_clear(&all_tags);
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

void
_handle_file_update(const char *path, const char *str)
{
  struct stat st;
  uuid_t uuid = { 0, 0, 0 };
  uuid_t uuid_linked = { 0, 0, 0 };
  char buf[PATH_MAX] = { 0 };

  if (stat(path, &st) != 0)
    return;

  if (file_uuid_get(path, &uuid) > 0)
    return;

  if (db_file_get(&uuid, buf) > 0)
    {
      msg(msg_warn, MSG_D_NOUUID, uuid_id_printf(&uuid));
      return;
    }

  if (st.st_nlink == 1 || S_ISDIR(st.st_mode))
    if (strncmp(buf, path, PATH_MAX) != 0)
      {
        db_file_update(path, &uuid);
        msg(msg_info, MSG_F_UPDATED, path);
      }

  if (st.st_nlink > 1)
    {
      if ((file_uuid_get(buf, &uuid_linked) == 0)
          && (uuid_linked.id == uuid.id))
        msg(msg_warn, MSG_F_LINKED, path, buf);
      else
        db_file_update(path, &uuid);
    }
}

/* #ifndef INLINE_TAGS */
void
_handle_file_migrate_to_db(const char *path, const char *unused)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t tags;
  int ret = 0;

  if (file_tags_get(path, &tags) > 0)
    return;

  if (db_file_add(path, &uuid) > 0)
    return;

  ret = db_tags_set(&uuid, &tags);

  data_clear(&tags);

  if (ret == 0 && !(flags & F_KEEPCONV))
    file_tags_clr(path);
}

void
_handle_file_migrate_from_db(const char *path, const char *unused)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t tags;
  int ret = 0;

  if (file_uuid_get(path, &uuid) > 0)
    return;

  if (db_tags_get(&uuid, &tags) > 0)
    return;

  ret = file_tags_set(path, &tags);

  data_clear(&tags);

  if (ret == 0 && !(flags & F_KEEPCONV))
    db_file_del(&uuid);
}
/* #endif */

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

  while ((opt = getopt(argc, argv, "rvhq" "cl" "a:d:s:" "f:F:" "T:" "u" "mMk")) != -1)
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
#ifndef INLINE_TAGS
        case 'm' : op = opt; str = optarg; handler = &_handle_file_migrate_to_db; break;
        case 'M' : op = opt; str = optarg; handler = &_handle_file_migrate_from_db; break;
        case 'k' : flags |= F_KEEPCONV; break;
#endif
        /* options */
        case 'v' : verbosity = log_extra; break;
        case 'q' : verbosity = log_quiet; break;
        case 'r' : flags |= F_RECURSE; break;
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
#ifndef INLINE_TAGS
      case 'm' :
      case 'M' :
#endif
        if (files.items < 1)
          exit(EXIT_FAILURE);
        while ((ret = data_items_walk(&files, &item)) > 0)
          (flags & F_RECURSE) ? _ftw(item, str, handler) : handler(item, str);
        break;
      default :
        usage(EXIT_FAILURE);
    }

  data_clear(&files);

  db_close();

  exit(EXIT_SUCCESS);
}
