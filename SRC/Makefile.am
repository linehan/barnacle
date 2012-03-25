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
arawak_SOURCES = arawak.c \
                   init/init.c          \
                   lib/redblack/rb.c    \
                   lib/sort/mergesort.c \
                   lib/sort/quicksort.c \
                   lib/morton.c         \
                   lib/magic.c          \
                   lib/fifo.c           \
                   lib/queue.c          \
                   lib/textutils.c      \
                   eng/state.c          \
                   eng/model/bytes.c    \
                   gen/dice.c           \
                   gen/mtwist.c         \
                   gen/name/name.c      \
                   geo/weather.c        \
                   geo/terrain.c        \
                   geo/map.c            \
                   gfx/palette.c        \
                   gfx/sprite.c         \
                   gfx/gfx.c            \
                   gfx/dock.c           \
                   gfx/wselect.c        \
                   gfx/fancy_menus.c    \
                   gen/perlin.c         \
                   gfx/term.c           \
                   gfx/stdpan.c         \
                   geo/path_mvc.c       \
                   txt/gloss_m.c        \
                   txt/gloss_v.c        \
                   txt/gloss_c.c        \
                   env/cloud.c          \
                   boat/boat_model.c           \
                   boat/boat_view.c           \
                   boat/boat_control.c           \
                   boat/compass.c\
                   noun/noun_model.c    \
                   noun/noun_view.c     \
                   noun/noun_control.c  \
                   verb/verb_model.c    \
                   verb/verb_view.c     \
                   verb/verb_control.c  \
                   verb/verbs.c         \
                   pan/ctrlpanels.c     \
                   pan/new_character.c  \
                   pan/button.c         \
                   pan/dialog.c         \
                   pan/test.c           \
                   pan/crewgen.c        \
                   loop/fsm.c           \
                   loop/loop.c          \
                   txt/rk.c             \
                   txt/psh.c
