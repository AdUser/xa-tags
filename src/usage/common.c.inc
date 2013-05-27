  fprintf(stdout,
  "Common options:\n"
  "  -h          This help.\n"
  "  -v          Be more verbose.\n"
  "  -q          Be less verbose.\n"
  "  -r          Handle files recursive.\n"
#if MODE != DUMB_TOOL
  "  -b <str>    Path to database. (default: autodetect)\n"
#endif
  "\n");
