AUTOMAKE_OPTIONS = foreign subdir-objects
#                      |            \
#         gprof       non-GNU     compile objects in the 
#           |                      indicated relative paths
# optimize  | warnings
# lvl 3 \   |    /    
CFLAGS=-O3 -pg -Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-pg -lm -lpthread -lmenu -lpanel -lform -lncursesw -lev
#       /    /      |        \         |       \       \       \__________
#      / math.h     |       menu     panels   forms     \                 \
#  gprof         POSIX     (curses)  (curses) (curses) ncurses with      libev   
#                threads                               wide character  
#                                                      support
bin_PROGRAMS = arawak
arawak_SOURCES = arawak.c               \
                   eng/init.c           \
                   lib/redblack/rb.c    \
                   lib/sort/mergesort.c \
                   lib/sort/quicksort.c \
                   lib/morton.c         \
                   lib/magic.c          \
                   lib/fifo.c           \
                   lib/textutils.c      \
                   lib/ufo.c            \
                   lib/stoc/matrix.c        \
                   lib/stoc/mersenne.c      \
                   lib/stoc/stoc.c          \
                   lib/stoc/noise.c         \
                   lib/stoc/dice.c          \
                   lib/stoc/shuffle.c       \
                   lib/stoc/smooth.c        \
                   lib/stoc/brnoise.c       \
                   eng/state.c          \
                   eng/model/bytes.c    \
                   map/weather.c        \
                   map/terrain.c        \
                   map/map.c            \
                   gfx/palette.c        \
                   gfx/sprite.c         \
                   gfx/gfx.c            \
                   gfx/multiwin.c       \
                   lib/ncolor/src/ncolor.c\
                   gfx/ui/dock.c        \
                   gfx/ui/stdpan.c      \
                   gfx/ui/titlecard.c   \
                   map/snake.c       \
                   txt/gloss_m.c        \
                   txt/gloss_v.c        \
                   txt/gloss_c.c        \
                   mob/mob_model.c      \
                   mob/mob_control.c    \
                   mob/boat/boat_model.c    \
                   mob/boat/boat_view.c     \
                   mob/boat/boat_control.c  \
                   mob/boat/compass.c       \
                   noun/noun_model.c    \
                   noun/noun_view.c     \
                   noun/noun_control.c  \
                   verb/verb_model.c    \
                   verb/verb_view.c     \
                   verb/verb_control.c  \
                   verb/verbs.c         \
                   test/test.c          \
                   test/testplex.c         \
                   test/tests/test_perlin.c         \
                   eng/fsm.c            \
                   eng/loop.c          
