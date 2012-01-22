AUTOMAKE_OPTIONS = foreign subdir-objects

CFLAGS=-Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-lm -lpthread -lmenu -lpanel -lncursesw -lev


bin_PROGRAMS = loam 
loam_SOURCES = loam.c \
	       cmd/control.c \
	       gen/dice.c \
	       gen/mtwist.c \
	       geo/weather.c \
	       geo/terrain.c \
	       gfx/gfx.c \
	       gfx/palette.c \
	       gfx/sprite.c \
	       mob/boat.c \
	       pan/instruments.c \
	       pan/menus.c \
	       pan/test.c


