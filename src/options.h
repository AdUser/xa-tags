struct
  {
    struct
      {
#ifdef ASYNC_DB_WRITE
        int commit_interval;
#endif
        char *path;
        bool readonly;
        bool uniq_tags;
      }
    db;
    struct
      {
        char *config;
        char *socket;
        char *pid;
        bool daemonize;
        enum loglevel loglevel;
      }
    daemon;
  }
opts;

