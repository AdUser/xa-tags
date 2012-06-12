enum loglevel { log_quiet, log_normal, log_extra, log_all };
enum msglevel { msg_error, msg_warn, msg_status, msg_info, msg_debug };

void msg(enum msglevel level, char *format, ...);
