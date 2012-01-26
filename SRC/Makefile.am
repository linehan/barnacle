AUTOMAKE_OPTIONS = foreign subdir-objects

CFLAGS=-Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-lm -lpthread -lmenu -lpanel -lncursesw -lev


bin_PROGRAMS = arawak
arawak_SOURCES = arawak.c \
                   cmd/control.c \
                   gen/dice.c \
                   gen/mtwist.c \
                   gen/perlin.c \
                   geo/weather.c \
                   geo/terrain.c \
                   gfx/palette.c \
                   gfx/sprite.c \
                   gfx/gfx.c \
                   mob/boat.c \
                   pan/instruments.c \
                   pan/menus.c \
                   pan/test.c \
                   txt/rk.c \
                   txt/psh.c



