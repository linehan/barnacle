AUTOMAKE_OPTIONS = foreign

CFLAGS=-Wall -Isrc/ -Isrc/lib -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-lm -lpthread -lmenu -lpanel -lncursesw -lev


bin_PROGRAMS = rope
rope_SOURCES = boat.c control.c dice.c dork.c palette.c sprite.c terrain.c test.c menus.c weather.c instruments.c lib/gfx.c lib/mtwist.c
