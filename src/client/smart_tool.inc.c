void
_handle_tag_add(const char *path, const char *str)
{
  uuid_t uuid = { 0, 0, 0 };
  data_t new_tags;
  data_t tags;

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&new_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      file_uuid_set(path, &uuid);
    }

  db_tags_get(&uuid, &tags);

  data_parse_tags(&new_tags, str);
  data_merge(&tags, &new_tags);
  data_clear(&new_tags);

#ifdef INLINE_TAGS
  file_tags_get(path, &new_tags);

  data_merge(&tags, &new_tags);

  file_tags_set(path, &tags);
#endif

  db_tags_set(&uuid, &tags);

  data_clear(&new_tags);
  data_clear(&tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  uuid_t uuid;
  data_t tags;
  data_t new_tags;

  ASSERT(path != NULL && str != NULL, MSG_M_NULLPTR);

  if (strlen(str) == 0)
    return; /* nothing to do*/

  memset(&new_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  if (file_uuid_get(path, &uuid) > 0)
    {
      if (db_file_add(path, &uuid) > 0)
        return;
      file_uuid_set(path, &uuid);
    }

  db_tags_get(&uuid, &tags);

  data_parse_tags(&new_tags, str);

  while(data_items_walk(&new_tags, &item) > 0)
    data_item_del(&tags, item);

  db_tags_set(&uuid, &tags);

#ifdef INLINE_TAGS
  file_tags_get(path, &tags);

  while(data_items_walk(&new_tags, &item) > 0)
    data_item_del(&tags, item);

  file_tags_set(path, &tags);
#endif

  data_clear(&new_tags);
  data_clear(&tags);

/*
  data_parse_tags(&new_tags, tags);

  FREE(buf);

  while (data_items_walk(&new_tags, &item) > 0)
    data_item_del(&old_tags, item);

  data_items_merge(&old_tags, ' ');

  len = (old_tags.len > 0) ? old_tags.len - 1 : 0 ;
  file_tags_set(path, old_tags.buf, len);
*/
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
  data_items_merge(&tags, ' ');
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

  if (file_uuid_get(path, &uuid) != 0)
    return;

  if (db_tags_get(&uuid, &tags) != 0)
    return;

  data_items_merge(&tags, ' ');
  printf("%s: %s\n", path, (tags.len > 0) ? tags.buf : "");

  data_clear(&tags);
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

int
main(int argc, char **argv)
{
  int ret = 0;
  data_t files;
  struct stat st;
  char opt = 0;
  char *str  = NULL;
  char *item = NULL;
  void (*handler)(const char *, const char *) = NULL;

  memset(&files, 0, sizeof(data_t));

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "hcl" "a:d:s:" "f:F:")) != -1)
    switch (opt)
      {
        case 'a' : handler = &_handle_tag_add; str = optarg; break;
        case 'd' : handler = &_handle_tag_del; str = optarg; break;
        case 's' : handler = &_handle_tag_set; str = optarg; break;
        case 'c' : handler = &_handle_tag_clr; break;
        case 'l' : handler = &_handle_tag_lst; break;
        case 'f' : handler = &_handle_file_search_tag;  str = optarg; break;
        case 'F' : handler = &_handle_file_search_path; str = optarg; break;
        case 'h' : usage(EXIT_SUCCESS); break;
        default  : usage(EXIT_FAILURE); break;
      }

  for (; optind < argc; optind++)
    {
      errno = 0;
      if (stat(argv[optind], &st) == 0)
        {
          item = realpath(argv[optind], NULL);
          data_item_add(&files, item, 0);
          FREE(item);
        }
      else
        printf("%s -- %s\n", argv[optind], strerror(errno));
    }

  if (files.items < 1)
    usage(EXIT_FAILURE);

  /* init */

  opts.db.path = db_find_path_user();
  db_open();

  while ((ret = data_items_walk(&files, &item)) > 0)
    handler(item, str);

  data_clear(&files);

  db_close();

  exit(EXIT_SUCCESS);
}
