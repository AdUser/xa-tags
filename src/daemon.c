/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "common.h"

enum loglevel verbosity = log_normal;

void
usage(int exitcode)
  {
    fprintf(stdout,
  "%s  version %u.%u\n",
  PROGNAME, VERSION_MAJOR, VERSION_MINOR);

    fprintf(stdout,
  "Usage: %s [options]\n"
  "\n"
  "Daemon options:\n"
  "  -h          Show this help and exit.\n"
  "  -c <file>   Config file.\n"
  "  -D          No daemonize.\n"
  "  -p <file>   Path to pid file.\n"
  "  -s <file>   Path to socket.\n"
  "\n", PROGNAME "-daemon");

    fprintf(stdout,
  "Database options:\n"
  "  -d <file>   Path to database\n"
  "  -r          Make database readonly\n"
    );

    exit(exitcode);
  }

void
cleanup(void)
{
  unlink(opts.daemon.socket);
}

static void
sigterm_handler(int sig, siginfo_t *siginfo, void *context)
{
  cleanup();
  exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
  {
    char opt;
    int sock_listen = 0;
    conn_t *conn = NULL;
    struct sigaction act;
    struct sockaddr_un sa;
    int sa_len = 0;
    char buf[4096];
    size_t buf_len = 0;

    while ((opt = getopt(argc, argv, "hcDp:s:" "dr")) != -1)
      switch (opt)
        {
          case 'd':
            STRNDUP(opts.db.path, optarg, MAXLINE);
            break;
          case 'r':
            opts.db.readonly = true;
            break;
          case 'c':
            STRNDUP(opts.daemon.config, optarg, MAXLINE);
            break;
          case 'D':
            opts.daemon.daemonize = false;
            break;
          case 'p':
            STRNDUP(opts.daemon.pid, optarg, MAXLINE);
            break;
          case 's':
            STRNDUP(opts.daemon.socket, optarg, MAXLINE);
            break;
          case 'h':
            usage(EXIT_SUCCESS);
            break;
          default :
            usage(EXIT_FAILURE);
            break;
        }

    if (!opts.daemon.socket)
      opts.daemon.socket = "./xa-tags-daemon.sock";

    unlink(opts.daemon.socket);

    if (!opts.db.path)
      opts.db.path = "./test.db";

    db_open();

    opts.daemon.loglevel = log_all;

    /* set cleanup actions on SIGTERM */
    memset(&act, 0x0, sizeof(struct sigaction));
    act.sa_sigaction = &sigterm_handler;
    sigaction(SIGTERM, &act, NULL);

    CALLOC(conn, 1, sizeof(conn_t));

    sock_listen = socket(AF_UNIX, SOCK_STREAM, 0);
    ASSERT(sock_listen != -1, "socket()\n");

    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, opts.daemon.socket);
    sa_len = offsetof(struct sockaddr_un, sun_path) + strlen(sa.sun_path);
    ASSERT(bind(sock_listen, (struct sockaddr *) &sa, sa_len) != -1, "bind()\n");
    ASSERT(listen(sock_listen, 1) == 0, "listen()\n");
    fprintf(stderr, "%s\n", strerror(errno));
    ASSERT((conn->fd = accept(sock_listen, NULL, 0)) != -1, "accept()\n");
    fprintf(stderr, "%s\n", strerror(errno));

    /* TODO: move this and similar to conn_on_init() */
    conn->errors.type = DATA_T_MSG;

    /* select() here */
    while (1)
      {
        if ((buf_len = read(conn->fd, buf, 4096)) > 0)
          {
            buf[buf_len] = '\0';
            buf_extend(&conn->rd, buf, buf_len);
            conn_on_read(conn);
          }
        if (conn->wr.len != 0)
          {
            conn_on_write(conn);
          }
        if (conn->flags & CONN_F_CLOSE)
          {
            close(conn->fd);
            break;
          }
      }

    cleanup();
    db_close();

    exit(EXIT_SUCCESS);
  }
