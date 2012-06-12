struct
  {
    struct
      {
        char *path;
        bool readonly;
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

