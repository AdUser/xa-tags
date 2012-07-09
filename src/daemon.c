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

void
usage(int exitcode)
  {
    fprintf(stdout,
  "Usage: xa-tags [options]\n"
  "\n"
  "Daemon options:\n"
  "  -h          Show this help and exit.\n"
  "  -c <file>   Config file.\n"
  "  -D          No daemonize.\n"
  "  -p <file>   Path to pid file.\n"
  "  -s <file>   Path to socket.\n"
  "\n");

    fprintf(stdout,
  "Database options:\n"
  "  -d <file>   Path to database\n"
  "  -r          Make database readonly\n"
    );

    exit(exitcode);
  }

int
main(int argc, char *argv[])
  {
    char opt;
    FILE *fh = stdin;
    yaml_parser_t parser;
    yaml_event_t event;

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

    if (!yaml_parser_initialize(&parser))
      fputs("Failed to initialize parser!\n", stderr);

    yaml_parser_set_input_file(&parser, fh);

    do
      {
        yaml_parser_parse(&parser, &event);
        switch (event.type)
          {
            case YAML_NO_EVENT: puts("No event!"); break;
            /* Stream start/end */
            case YAML_STREAM_START_EVENT: puts("STREAM START"); break;
            case YAML_STREAM_END_EVENT:   puts("STREAM END");   break;
            /* Block delimeters */
            case YAML_DOCUMENT_START_EVENT: puts("<b>Start Document</b>"); break;
            case YAML_DOCUMENT_END_EVENT:   puts("<b>End Document</b>");   break;
            case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
            case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
            case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
            case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");    break;
            /* Data */
            case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
            case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); break;
          }
        if (event.type != YAML_STREAM_END_EVENT)
         yaml_event_delete(&event);
      }
    while(event.type != YAML_STREAM_END_EVENT);

    yaml_event_delete(&event);
    yaml_parser_delete(&parser);

    fclose(fh);
    exit(EXIT_SUCCESS);
  }

