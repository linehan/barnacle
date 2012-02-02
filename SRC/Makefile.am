AUTOMAKE_OPTIONS = foreign subdir-objects
#                      |            \
#         gprof       non-GNU     compile objects in the 
#           |                      indicated relative paths
# optimize  | warnings
# lvl 3 \   |    /
CFLAGS=-O3 -pg -Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-lm -lpthread -lmenu -lpanel -lncursesw -lev
#       /      |        \         |       \       |_____________
#  math.h      |       menu     panels     \                    \
#          POSIX     (curses)  (curses)   ncurses with          libev   
#          threads                        wide character   (event handler)
#                                         support

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
                   gfx/term.c \
                   mob/boat.c \
                   pan/instruments.c \
                   pan/menus.c \
                   pan/test.c \
                   txt/rk.c \
                   txt/psh.c
