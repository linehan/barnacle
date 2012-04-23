#pragma once
#ifndef __MAP_H
#define __MAP_H
/*
 *
 * map.h -- objects and methods for graphical representation of terrain 
 *
 */
#include "../com/arawak.h"
#include "../eng/bytes.h"
#include "../gfx/gfx.h"

#include "../lib/ufo.h"
#include "../mob/mob.h"
#include "tag.h"
#include "arena.h"
#include "asset.h"


#define WORLD_HEIGHT   (LINES*3)
#define WORLD_WIDTH    (COLS*3)
#define FIELD_HEIGHT   16
#define FIELD_WIDTH    18
#define FULLSCREEN     LINES+1, COLS+1
#define NASSETS        10
#define PLATE(map,tag) PEEK(((map)->L[(tag)]))


/*
 * A map_t holds all the data necessary to fill a screen with terrain.
 * It contains matrices which hold "tag" or "label" values cooresponding
 * to a set of tokens which can be read by the raster parser, which
 * interprets the label and takes a corresponding action, such as drawing
 * a tile on the screen, or calling a function.
 */
struct map_t {
        /*----------------------------- Graphics */
        WINDOW *win;
        PANEL  *pan;
        struct multiwin_t *L[NLAYERS];
        struct multiwin_t *W;
        /*----------------------------- Logic */
        struct matrix_t *mx;
        struct matrix_t *hook;
        double **pmap;
        struct ufo_t ufo;
        struct asset_t *asset[NASSETS];
};


/*
 * The mapbook is a unique object that serves as a scaffold for various
 * rendering and UI functions. It contains pointers to the two map screens
 * that may be active at any given time: the world map, and the field map,
 * which is derived from the world map.
 */
struct mapbook {
        /*---------------------------- Map members */
        struct map_t *world;
        struct map_t *field;
        struct map_t *extra;
        struct map_t *active;
        /*---------------------------- Toggle */
        bool field_is_active;
        bool extra_is_active;   
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
struct mapbook *MAPBOOK;
#define WORLD  (MAPBOOK->world)  /* Accesses the world map */
#define FIELD  (MAPBOOK->field)  /* Accesses the field map */
#define EXTRA  (MAPBOOK->extra)
#define ACTIVE (MAPBOOK->active) /* Accesses whichever is active */


enum map_label_opts { MAP_DOSMOOTH, MAP_NOSMOOTH };



/*Map creation functions
````````````````````````````````````````````````````````````````````````````` */
struct map_t   *new_map(int h, int w);
struct mapbook *new_mapbook(void);
struct map_t   *new_inset(struct map_t *map, int h, int w, int y, int x);

/* Map drawing and labeling functions
````````````````````````````````````````````````````````````````````````````` */
void map_gen(struct map_t *map, double **pmap, int opt);
void map_label(struct map_t *map, int opt);
void map_render(void *mymap);

/* Map interaction functions  
````````````````````````````````````````````````````````````````````````````` */
int  map_hit(struct map_t *map, struct rec_t *rec);
void map_trigger(struct map_t *map, struct mob_t *mob);
void map_roll(struct map_t *map, int dir);
void map_swap(void);
void map_set_extra(void *mymap);


#endif

