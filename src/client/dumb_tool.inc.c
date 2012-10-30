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
  data_items_merge(&tags, ' ');

  printf("%s: %s\n", path, (tags.len > 0) ? tags.buf : "");
 }

void
_handle_search_by_tag(const char *path, const char *str)
{
  data_t tags;
  char *tmp = NULL;
  char *buf = NULL;
  size_t buf_size = 1024;
  struct stat st;

  memset(&tags, 0, sizeof(data_t));
  CALLOC(buf, 1, buf_size * sizeof(char));

  data_parse_tags(&tags, str);

  stat(path, &st);

  if (S_ISREG(st.st_mode))
    {
      errno = 0;
      while (getxattr(path, XATTR_TAGS, buf, buf_size - 1) < 0 && errno == ERANGE)
        {
          buf_size *= 2;
          tmp = buf;
          REALLOC(buf, tmp, buf_size);
          errno = 0;
        }

      if (errno == 0)
        {
          while (data_items_walk(&tags, &tmp) > 0)
            if (strstr(buf, tmp) == NULL)
              goto done;

          /* If loop above completes without return, this means,  *
           * that 'buf' contain all tokens from 'tags'. Print it. */
          _handle_tag_lst(path, NULL);
        }
    }

  /* TODO: ftw for dir */

  done:
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

  while ((opt = getopt(argc, argv, "hcl" "a:d:s:f:")) != -1)
    switch (opt)
      {
        case 'a' : handler = &_handle_tag_add; tags = optarg; break;
        case 'd' : handler = &_handle_tag_del; tags = optarg; break;
        case 's' : handler = &_handle_tag_set; tags = optarg; break;
        case 'c' : handler = &_handle_tag_clr; break;
        case 'l' : handler = &_handle_tag_lst; break;
        case 'f' : handler = &_handle_search_by_tag; tags = optarg; break;
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
