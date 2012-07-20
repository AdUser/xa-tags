  "/home/user/test / file.png\n" /* pass */ \
  "home/user/test/file.png\n"    /* fail */ \
  "~/test/file.png\n"            /* pass */ \
  "file.png\n"                   /* fail */ \
  "  ~/test/file.png  \n"        /* pass */ \
  "~/test/\n"                    /* pass */ \
  "~/test/file.png"              /* pass */ ;
