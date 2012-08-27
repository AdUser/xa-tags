void
usage(int exitcode)
{
  fprintf(stdout,
  "%s  version %u.%u\n",
  PROGNAME, VERSION_MAJOR, VERSION_MINOR);

  fprintf(stdout,
  "Usage: %s [options] <file> [<file>]\n"
  "\n"
  "Basic usage:\n"
  "  -h          This help.\n"
  "  -a          Add tag.\n"
  "  -d          Delete tag.\n"
  "  -c          Delete all tags (clear).\n"
  "  -s          Set for file this set of tags.\n"
  "  -l          List tags.\n"
  "\n", PROGNAME);

  exit(exitcode);
}
