CFLAGS = -Wall -Wextra -pedantic

SOURCES = daemon.c log.c proto.c
DEFINES = -DHAVE_GETTEXT

all: Makefile $(SOURCES)
	gcc $(CFLAGS) $(SOURCES) $(DEFINES) -o "test"

# vim: ts=4:noet
