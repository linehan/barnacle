#pragma once
#ifndef __MAP_H
#define __MAP_H

#include <stdint.h>
#include "../gfx/gfx.h"
#include "../eng/bytes.h"
#include "../lib/ufo.h"
#include "../mob/mob.h"
#include "tag.h"
#include "arena.h"

#define WORLD_HEIGHT (LINES*3)
#define WORLD_WIDTH (COLS*3)
#define FIELD_HEIGHT 16
#define FIELD_WIDTH 18

/*
 * The map type holds all the data necessary to fill a screen with
 * a graphical representation of terrain.
 */
struct map_t {
        WINDOW *win;
        PANEL  *pan;
        struct multiwin_t *L[NLAYERS];
        struct multiwin_t *W;
        struct matrix_t *mx;
        struct matrix_t *hook;
        struct ufo_t ufo;
        struct asset_t *asset[10];
        double **pmap;
};

struct mapbook {
        /*---------------------------- screens */
        struct map_t *world;
        struct map_t *field;
        struct map_t *active;
        /*---------------------------- toggle */
        bool field_is_active;
        /*---------------------------- methods */
        void (*render)(void *self);
        void (*restack)(void *self);
        void (*hide)(void *self);
        void (*show)(void *self);
};


//extern struct map_t *GLOBE; // Global map.
//struct map_t *WORLD;

struct mapbook *MAPBOOK;
#define WORLD (MAPBOOK->world)
#define FIELD (MAPBOOK->field)
#define ACTIVE (MAPBOOK->active)


struct asset_storage {
        int h;
        int w;
        short *color;
        wchar_t **wch;
};


struct asset_t {
        int h;
        int w;
        int y;
        int x;
        int l;
        WINDOW *win;
        PANEL  *pan;
        cchar_t *cch;
        void (*put)(void *map, int idx, int y, int x);
        void (*hide)(void *self);
        void (*show)(void *self);
        void (*trigger)(void);
};


#define PLATE(map, tag) PEEK(((map)->L[(tag)]))

enum map_label_opts { MAP_DOSMOOTH, MAP_NOSMOOTH };

struct map_t *new_map(int h, int w);
int           map_hit(struct map_t *map, struct rec_t *rec);
void          map_gen(struct map_t *map, double **pmap, int opt);
void         map_roll(struct map_t *map, int dir);
void map_trigger(struct map_t *map, struct mob_t *mob);
void map_swap(void);
struct mapbook *new_mapbook(void);


#endif

