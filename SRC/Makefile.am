AUTOMAKE_OPTIONS = foreign subdir-objects
#                      |            \
#         gprof       non-GNU     compile objects in the 
#           |                      indicated relative paths
# optimize  | warnings
# lvl 3 \   |    /    
CFLAGS=-O3 -pg -Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-lm -lpthread -lmenu -lpanel -lform -lncursesw -lev
#       /      |        \         |       \       \       |_____________
#  math.h      |       menu     panels   forms     \                    \
#          POSIX     (curses)  (curses) (curses) ncurses with          libev   
#          threads                               wide character  (event handler)
#                                                support
bin_PROGRAMS = arawak
arawak_SOURCES = arawak.c \
                   lib/redblack/rb.c \
                   lib/sort/mergesort.c \
                   lib/sort/quicksort.c \
                   lib/magic.c       \
                   eng/state.c       \
                   eng/model/bytes.c \
                   cmd/control.c     \
                   gen/dice.c        \
                   gen/mtwist.c      \
                   gen/name/name.c   \
                   gen/wave.c        \
                   geo/weather.c     \
                   geo/terrain.c     \
                   geo/map.c         \
                   gfx/palette.c     \
                   gfx/sprite.c      \
                   gfx/gfx.c         \
                   gfx/dock.c        \
                   gfx/wselect.c     \
                   gfx/fancy_menus.c \
                   gen/perlin.c      \
                   gfx/term.c        \
                   env/deck.c        \
                   env/cloud.c       \
                   mob/mob.c         \
                   mob/boat.c        \
                   noun/noun_model.c   \
                   noun/noun_view.c    \
                   noun/noun_control.c \
                   verb/verb_model.c \
                   verb/verb_view.c \
                   pan/ctrlpanels.c  \
                   pan/new_character.c \
                   pan/instruments.c \
                   pan/button.c      \
                   pan/dialog.c      \
                   pan/test.c        \
                   pan/crewgen.c     \
                   txt/rk.c          \
                   txt/psh.c
