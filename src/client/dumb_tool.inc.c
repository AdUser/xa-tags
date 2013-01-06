void
_handle_tag_add(const char *path, const char *str)
{
  data_t new_tags;
  data_t tags;

  memset(&new_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  file_tags_get(path, &tags);

  data_parse_tags(&new_tags, str);
  data_merge(&tags, &new_tags);

  data_items_merge(&tags, ' ');

  file_tags_set(path, &tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  data_t new_tags;
  data_t tags;

  memset(&new_tags, 0, sizeof(data_t));
  memset(&tags,     0, sizeof(data_t));

  file_tags_get(path, &tags);

  data_parse_tags(&new_tags, str);

  while (data_items_walk(&new_tags, &item) > 0)
    data_item_del(&tags, item);

  data_items_merge(&tags, ' ');

  file_tags_set(path, &tags);
}

void
_handle_tag_clr(const char *path, const char *unused)
{
  file_tags_clr(path);
}

void
_handle_tag_set(const char *path, const char *str)
{
  data_t tags;

  memset(&tags, 0, sizeof(data_t));

  if (strlen(str) == 0)
    {
      _handle_tag_clr(path, NULL);
      return;
    }

  data_parse_tags(&tags, str);

  file_tags_set(path, &tags);
}

void
_handle_tag_lst(const char *path, const char *unused)
{
  data_t tags;

  memset(&tags, 0, sizeof(data_t));

  file_tags_get(path, &tags);

  if (tags.items == 0)
    return;

  data_items_merge(&tags, ' ');

  printf("%s: %s\n", path, (tags.len > 0) ? tags.buf : "");
 }

void
_handle_search_by_tag(const char *path, const char *str)
{
  data_t search_tags;
  char *tmp = NULL;
  char *buf = NULL;
  size_t buf_size = 1024;
  ssize_t attr_size = 0;
  bool match = false;
  const int fts_flags = FTS_PHYSICAL | FTS_NOCHDIR;
  FTS *fts = NULL;
  FTSENT *ftsent = NULL;
  char *fts_argv[2];

  memset(&search_tags, 0, sizeof(data_t));
  CALLOC(buf, 1, buf_size * sizeof(char));

  data_parse_tags(&search_tags, str);

  fts_argv[0] = (char * const) path;
  fts_argv[1] = NULL;

  if ((fts = fts_open(fts_argv, fts_flags, NULL)) == NULL)
    msg(msg_error, MSG_F_FAILOPEN, path);

  while ((ftsent = fts_read(fts)) != NULL)
    if (ftsent->fts_info & (FTS_DP | FTS_D) ||
        ftsent->fts_info & FTS_F)
      {
        /* not a function, because we can reduce a lot of calls *
         * of malloc() and free() with one lazy growing 'buf'   */
        errno = 0;
        attr_size = getxattr(ftsent->fts_path, XATTR_TAGS, buf, buf_size - 1);
        while (attr_size < 0 && errno == ERANGE)
          {
            buf_size *= 2;
            tmp = buf;
            REALLOC(buf, tmp, buf_size);
            errno = 0;
            attr_size = getxattr(ftsent->fts_path, XATTR_TAGS, buf, buf_size - 1);
          }
        buf[attr_size + 1] = '\0';
        if (errno == 0)
          {
            match = true;
            tmp = NULL;
            while (data_items_walk(&search_tags, &tmp) > 0)
              if (strstr(buf, tmp) == NULL)
                match = false;

            if (match)
              _handle_tag_lst((ftsent->fts_path), NULL);
          }
      }

  fts_close(fts);

  free(buf);
}

int
main(int argc, char **argv)
{
  int ret = 0;
  data_t files;
  struct stat st;
  char opt = 0;
  char *tags = NULL;
  char *item = NULL;
  void (*handler)(const char *, const char *) = NULL;

  memset(&files, 0, sizeof(data_t));

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "vqh" "cl" "a:d:s:f:")) != -1)
    switch (opt)
      {
        /* operations */
        case 'a' : handler = &_handle_tag_add; tags = optarg; break;
        case 'd' : handler = &_handle_tag_del; tags = optarg; break;
        case 's' : handler = &_handle_tag_set; tags = optarg; break;
        case 'c' : handler = &_handle_tag_clr; break;
        case 'l' : handler = &_handle_tag_lst; break;
        case 'f' : handler = &_handle_search_by_tag; tags = optarg; break;
        /* options */
        case 'v' : verbosity = log_extra; break;
        case 'q' : verbosity = log_quiet; break;
        case 'h' : usage(EXIT_SUCCESS); break;
        default  : usage(EXIT_FAILURE); break;
      }

  for (; optind < argc; optind++)
    {
      errno = 0;
      if (stat(argv[optind], &st) == 0)
        data_item_add(&files, argv[optind], 0);
      else
        printf("%s -- %s\n", argv[optind], strerror(errno));
    }

  if (files.items < 1)
    usage(EXIT_FAILURE);

  /* init */
  while ((ret = data_items_walk(&files, &item)) > 0)
    handler(item, tags);

  data_clear(&files);

  exit(EXIT_SUCCESS);
}
