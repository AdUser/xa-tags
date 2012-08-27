void
_handle_tag_add(const char *path, const char *tags)
{
  char *buf = NULL;
  data_t new_tags;
  data_t old_tags;

  memset(&new_tags, 0, sizeof(data_t));
  memset(&old_tags, 0, sizeof(data_t));

  file_tags_get(path, &buf);

  if (buf != NULL)
    data_parse_tags(&old_tags, buf);

  data_parse_tags(&new_tags, tags);

  FREE(buf);

  data_merge(&old_tags, &new_tags);
  data_items_merge(&old_tags, ' ');

  file_tags_set(path, old_tags.buf, strlen(old_tags.buf));
}

void
_handle_tag_del(const char *path, const char *tags)
{
  char *buf = NULL;
  char *item = NULL;
  data_t new_tags;
  data_t old_tags;
  size_t len = 0;

  memset(&new_tags, 0, sizeof(data_t));
  memset(&old_tags, 0, sizeof(data_t));

  file_tags_get(path, &buf);

  if (buf != NULL)
    data_parse_tags(&old_tags, buf);

  data_parse_tags(&new_tags, tags);

  FREE(buf);

  while (data_items_walk(&new_tags, &item) > 0)
    data_item_del(&old_tags, item);

  data_items_merge(&old_tags, ' ');

  len = (old_tags.len > 0) ? old_tags.len - 1 : 0 ;
  file_tags_set(path, old_tags.buf, len);
}

void
_handle_tag_clr(const char *path, const char *tags)
{
  errno = 0;
  removexattr(path, XATTR_TAGS);
  switch (errno)
    {
      case ENOATTR :
        break;
      default :
        msg(msg_warn, "%s: %s\n", path, strerror(errno));
        break;
    }
}

void
_handle_tag_set(const char *path, const char *tags)
{
  data_t new_tags;
  size_t len = 0;

  memset(&new_tags, 0, sizeof(data_t));

  if (strlen(tags) == 0)
    {
      _handle_tag_clr(path, NULL);
      return;
    }

  data_parse_tags(&new_tags, tags);
  data_items_merge(&new_tags, ' ');

  len = (new_tags.len > 0) ? new_tags.len - 1 : 0 ;
  file_tags_set(path, new_tags.buf, len);
}

void
_handle_tag_lst(const char *path, const char *unused)
{
  char *tags = NULL;
  size_t size = 0;

  size = file_tags_get(path, &tags);

  printf("%s: %s\n", path, (size > 0) ? tags : "");

  FREE(tags);
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

  while ((opt = getopt(argc, argv, "hcl" "a:d:s:")) != -1)
    switch (opt)
      {
        case 'a' : handler = &_handle_tag_add; tags = optarg; break;
        case 'd' : handler = &_handle_tag_del; tags = optarg; break;
        case 's' : handler = &_handle_tag_set; tags = optarg; break;
        case 'c' : handler = &_handle_tag_clr; break;
        case 'l' : handler = &_handle_tag_lst; break;
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
