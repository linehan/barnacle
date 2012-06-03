#ifndef __MAP_H
#define __MAP_H
/*
 * map.h -- objects and methods for graphical representation of terrain 
 */
#include "../com/barnacle.h"
#include "../eng/bytes.h"
#include "../gfx/gfx.h"
#include "../lib/matrix.h"
#include "../lib/list.h"
#include "../lib/pos.h"
#include "terrain.h"
#include "tag.h"
#include "special/arena.h"
#include "door.h"

enum map_class { MAP2DF, MAP_CAVE };
enum map_page  { MAP_WORLD, MAP_EXTRA, MAP_FIELD };

#define WORLD_HEIGHT   (LINES*3)
#define WORLD_WIDTH    (COLS*3)
#define FIELD_HEIGHT   16
#define FIELD_WIDTH    18
#define FULLSCREEN     LINES+1, COLS+1
#define PLATE(map,tag) PEEK(((map)->L[(tag)]))


/**
 * A map_t holds all the data necessary to fill a screen with terrain.
 * It contains matrices which hold "tag" or "label" values cooresponding
 * to a set of tokens which can be read by the raster parser, which
 * interprets the label and takes a corresponding action, such as drawing
 * a tile on the screen, or calling a function.
 */
struct map_t {
        uint32_t id;
        enum map_class tag;
        /*----------------------------- Graphics */
        WINDOW *win;
        PANEL  *pan;
        struct multiwin_t *L[3];
        struct multiwin_t *W;
        /*----------------------------- Logic */
        struct matrix_t *tile;
        struct matrix_t *mobs;
        struct matrix_t *door;
        struct matrix_t *item;
        struct matrix_t *elev;
        double **pmap;
        struct pos_t *pos;
        struct list_node node;
};


/**
 * The mapbook is a unique object that serves as a scaffold for various
 * rendering and UI functions. It contains pointers to the two map screens
 * that may be active at any given time: the world map, and the field map,
 * which is derived from the world map.
 */
struct mapbook_t {
        /*---------------------------- Map members */
        struct list_head zoom;
        struct map_t *active;
        struct map_t *world;
        struct map_t *field;
        struct map_t *extra;
        /*---------------------------- Cycle */
        enum map_page page;
        /*---------------------------- Methods */
        void (*render)(void *self);
        void (*restack)(void *self);
        void (*hide)(void *self);
        void (*show)(void *self);
};


/*
 * This is the mapbook pointer; it has global scope and is the only
 * mapbook that should be active at any given time.
 */
struct mapbook_t *MAPBOOK;
#define WORLD  (MAPBOOK->world)  /* Accesses the world map */
#define FIELD  (MAPBOOK->field)  /* Accesses the field map */
#define EXTRA  (MAPBOOK->extra)
#define ACTIVE (MAPBOOK->active) /* Accesses whichever is active */

enum map_label_opts { MAP_DOSMOOTH, MAP_NOSMOOTH };

static bool gravity_enabled = true;


/*Map creation functions
````````````````````````````````````````````````````````````````````````````` */
struct map_t   *new_map(int h, int w);
struct mapbook_t *new_mapbook(void);
struct map_t   *new_inset(struct map_t *map, int h, int w, int y, int x);

/* Map drawing and labeling functions
````````````````````````````````````````````````````````````````````````````` */
void map_gen(struct map_t *map, double **pmap, int opt);
void map_label(struct map_t *map, int opt);
void map_render(void *mymap);

/* Map interaction functions  
````````````````````````````````````````````````````````````````````````````` */
bool map_hit(struct map_t *map, struct noun_t *noun);
bool mob_hit(struct map_t *map, struct noun_t *noun);
void map_scroll(struct map_t *map, int dir);
void map_swap(void);
void map_cycle(void);
void map_set_extra(void *mymap);


#endif

