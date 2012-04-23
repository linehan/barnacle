AUTOMAKE_OPTIONS = foreign subdir-objects
#                      |            \
#         gprof       non-GNU        compile objects in the 
#           |                        indicated relative paths
# optimize  | warnings  debug info
# lvl 3 \   |    /     /  
CFLAGS=-O3 -pg -Wall -g -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-pg -lm -lrt -lpthread -lmenu -lpanel -lform -lncursesw -lev
#       /    /    |    |         \        |       \        \       \_______
#      / math.h   |    |        menu     panels   forms     \              \
#  gprof        time  POSIX    (curses)  (curses) (curses) ncurses with   libev   
#                     threads                              wide character  
#                                                          support
bin_PROGRAMS = arawak
arawak_SOURCES = arawak.c                       \
                   (wildcard lib/ccan/*/*.c)    \
                   eng/init.c                   \
                   lib/redblack/rb.c            \
                   lib/sort/mergesort.c         \
                   lib/sort/quicksort.c         \
                   lib/morton.c                 \
                   lib/magic.c                  \
                   lib/ncolor/src/ncolor.c      \
                   lib/fifo.c                   \
                   lib/textutils.c              \
                   lib/ufo.c                    \
                   lib/stoc/mersenne.c          \
                   lib/stoc/stoc.c              \
                   lib/stoc/noise.c             \
                   lib/stoc/dice.c              \
                   lib/stoc/shuffle.c           \
                   lib/stoc/smooth.c            \
                   lib/stoc/brnoise.c           \
                   lib/matrix.c                 \
                   eng/bytes.c                  \
                   eng/fsm.c                    \
                   eng/loop.c                   \
                   thr/thr_pool.c               \
                   gfx/palette.c                \
                   gfx/sprite.c                 \
                   gfx/gfx.c                    \
                   gfx/multiwin.c               \
                   gfx/ui/dock.c                \
                   gfx/ui/stdpan.c              \
                   gfx/ui/stdmenu.c             \
                   gfx/ui/titlecard.c           \
                   gfx/fall.c                   \
                   map/terrain.c                \
                   map/map.c                    \
                   map/cell.c                   \
                   map/snake.c                  \
                   map/arena.c                  \
                   map/asset.c                  \
                   map/sweet_flow.c             \
                   ai/a_star.c                  \
                   txt/gloss_m.c                \
                   txt/gloss_v.c                \
                   txt/gloss_c.c                \
                   mob/mob.c                    \
                   noun/noun_model.c            \
                   noun/noun_discrim.c          \
                   noun/noun_view.c             \
                   noun/noun_control.c          \
                   noun/noun_widgets.c          \
                   noun/types/boat/boat.c       \
                   noun/types/boat/compass.c    \
                   noun/types/creature/creature.c   \
                   verb/verb_model.c            \
                   verb/verb_view.c             \
                   verb/verb_control.c          \
                   verb/verbs.c                 \
                   test/test.c                  

