/* global variables  */
search_t *search = NULL;

/* handlers */
void
_handle_tag_add(const char *path, const char *str)
{
  list_t new_tags;
  list_t tags;

  memset(&new_tags, 0, sizeof(list_t));
  memset(&tags,     0, sizeof(list_t));

  file_tags_get(path, &tags);

  list_parse_tags(&new_tags, str);
  list_merge(&tags, &new_tags);

  list_items_merge(&tags, ' ');

  file_tags_set(path, &tags);

  list_clear(&tags);
  list_clear(&new_tags);
}

void
_handle_tag_del(const char *path, const char *str)
{
  char *item = NULL;
  list_t new_tags;
  list_t tags;

  memset(&new_tags, 0, sizeof(list_t));
  memset(&tags,     0, sizeof(list_t));

  if (file_tags_get(path, &tags) > 0)
    return;

  list_parse_tags(&new_tags, str);

  while (list_items_walk(&new_tags, &item) > 0)
    list_item_del(&tags, item);

  list_items_merge(&tags, ' ');

  file_tags_set(path, &tags);

  list_clear(&tags);
  list_clear(&new_tags);
}

void
_handle_tag_clr(const char *path, const char *unused)
{
  file_tags_clr(path);
}

void
_handle_tag_set(const char *path, const char *str)
{
  list_t tags;

  memset(&tags, 0, sizeof(list_t));

  if (strlen(str) == 0)
    {
      _handle_tag_clr(path, NULL);
      return;
    }

  list_parse_tags(&tags, str);

  file_tags_set(path, &tags);

  list_clear(&tags);
}

void
_handle_tag_lst(const char *path, const char *unused)
{
  list_t tags;

  memset(&tags, 0, sizeof(list_t));

  if (file_tags_get(path, &tags) > 0)
    return;

  list_items_merge(&tags, ' ');

  printf(COMMON_MSG_FMTN, path, (tags.len > 0) ? tags.buf : "");

  list_clear(&tags);
}

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
_handle_search_by_tag(const char *path, const char *str)
{
  char *file_tags = NULL;

  if (file_tags_get_bulk(path, &file_tags) <= 0)
    return;

  if (search == NULL)
    _search_init(str);

  if (search_match(search, file_tags) <= 0)
    return;

  if (verbosity > log_normal)
    _handle_tag_lst(path, NULL);

  if (verbosity <= log_normal)
    printf("%s\n", path);

  FREE(file_tags);
}

int
main(int argc, char **argv)
{
  int ret = 0;
  list_t files;
  struct stat st;
  char opt = 0;
  char *tags = NULL;
  char *item = NULL;
  void (*handler)(const char *, const char *) = NULL;

  memset(&files, 0, sizeof(list_t));

#ifdef HAVE_GETTEXT
  setlocale (LC_ALL, "");
  bindtextdomain(PROGNAME, LOCALEDIR);
  textdomain(PROGNAME);
#endif

  if (argc < 2)
    usage(EXIT_FAILURE);

  while ((opt = getopt(argc, argv, "rvqh" "cl" "a:d:s:" "f:")) != -1)
    switch (opt)
      {
        /* operations */
        case 'a' : handler = &_handle_tag_add; tags = optarg; break;
        case 'd' : handler = &_handle_tag_del; tags = optarg; break;
        case 's' : handler = &_handle_tag_set; tags = optarg; break;
        case 'c' : handler = &_handle_tag_clr; break;
        case 'l' : handler = &_handle_tag_lst; break;
        case 'f' : handler = &_handle_search_by_tag; tags = optarg; flags |= F_RECURSE; break;
        /* options */
        case 'v' : verbosity = log_extra; break;
        case 'q' : verbosity = log_quiet; break;
        case 'r' : flags |= F_RECURSE; break;
        case 'h' : usage(EXIT_SUCCESS); break;
        default  : usage(EXIT_FAILURE); break;
      }

  for (; optind < argc; optind++)
    {
      errno = 0;
      if (stat(argv[optind], &st) == 0)
        list_item_add(&files, argv[optind], 0);
      else
        printf(COMMON_ERR_FMTN, argv[optind], strerror(errno));
    }

  if (files.items < 1)
    usage(EXIT_FAILURE);

  /* init */
  while ((ret = list_items_walk(&files, &item)) > 0)
    (flags & F_RECURSE) ? _ftw(item, tags, handler) : handler(item, tags);

  list_clear(&files);
  FREE(search);

  exit(EXIT_SUCCESS);
}
