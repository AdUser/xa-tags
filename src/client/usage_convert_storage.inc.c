  fprintf(stdout,
"Change tags storage:\n"
"  -m          Move embedded tags to database.\n"
"  -M          Move tags from database to separate xattr.\n"
"  -k          Don't delete converted tags from old place.\n"
"              Use with caution, this leads to the fact, that you have\n"
"              two sets of tags: in xattr and database.\n"
"");
