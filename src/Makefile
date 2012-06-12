CFLAGS = -Wall -Wextra -pedantic -lyaml

SOURCES = main.c log.c
DEFINES = -DHAVE_GETTEXT

all: Makefile $(SOURCES)
	gcc $(CFLAGS) $(SOURCES) $(DEFINES) -o "test"

# vim: ts=4:noet
