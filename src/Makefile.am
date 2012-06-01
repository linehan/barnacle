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
bin_PROGRAMS = barnacle 
barnacle_SOURCES = barnacle.c                   \
                   eng/init.c                   \
                   lib/fsm/fsm.c                \
                   lib/redblack/rb.c            \
                   lib/matrix.c                 \
                   lib/sort/mergesort.c         \
                   lib/sort/quicksort.c         \
                   lib/morton.c                 \
                   lib/magic.c                  \
                   lib/fifo.c                   \
                   lib/textutils.c              \
                   lib/pos.c                    \
                   lib/rec.c                    \
                   lib/stoc/mersenne.c          \
                   lib/stoc/stoc.c              \
                   lib/stoc/noise.c             \
                   lib/stoc/dice.c              \
                   lib/stoc/shuffle.c           \
                   lib/stoc/smooth.c            \
                   lib/stoc/brnoise.c           \
                   eng/fsm.c                    \
                   eng/loop.c                   \
                   eng/error.c                  \
                   gfx/palette.c                \
                   gfx/sprite.c                 \
                   gfx/gfx.c                    \
                   gfx/multiwin.c               \
                   gfx/ui/dock.c                \
                   gfx/ui/stdpan.c              \
                   gfx/ui/stdmenu.c             \
                   gfx/ui/titlecard.c           \
                   gfx/ui/menu_inventory.c      \
                   gfx/ui/notify.c              \
                   map/terrain.c                \
                   map/map.c                    \
                   map/cell.c                   \
                   map/door.c                   \
                   map/special/arena.c          \
                   map/special/cave.c           \
                   map/flow.c                   \
                   map/light.c                  \
                   ai/astar.c                   \
                   txt/gloss.c                  \
                   txt/effects.c                \
                   noun/noun.c                  \
                   noun/animate.c               \
                   noun/combat.c                \
                   noun/spawn.c                 \
                   noun/models.c                \
                   noun/models/human.c          \
                   noun/models/hopper.c         \
                   item/item.c                  \
                   item/tools.c                 \
                   item/edible.c                \
                   town/bldg.c                  \
                   test/test.c                  \
                   test/looptest.c              

