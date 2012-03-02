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
                   lib/redblack/rb.c \
                   lib/sort/mergesort.c \
                   lib/sort/quicksort.c \
                   itm/domain.c      \
                   itm/itm.c         \
                   cmd/control.c     \
                   gen/dice.c        \
                   gen/mtwist.c      \
                   gen/wave.c        \
                   geo/weather.c     \
                   geo/terrain.c     \
                   geo/map.c         \
                   gfx/palette.c     \
                   gfx/sprite.c      \
                   gfx/gfx.c         \
                   gen/perlin.c      \
                   gfx/term.c        \
                   env/deck.c        \
                   env/cloud.c       \
                   mob/mob.c         \
                   mob/boat.c        \
                   pan/instruments.c \
                   pan/dialog.c      \
                   pan/menus.c       \
                   pan/test.c        \
                   txt/rk.c          \
                   txt/psh.c
