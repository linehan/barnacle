#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include "../com/barnacle.h"
#include "../map/map.h"
#include "../lib/fsm/fsm.h"
#include "../ai/astar.h"
#include "animate.h"
#include "combat.h"
#include "stats.h"
#include "models.h"


/* The noun type
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t {
        /* State ------------------------------------------*/
        const char *name;            // String identifier
        uint32_t id;              // Hashed identifier
        uint32_t vitals;          // The state word
        bool is_mobile;           // Rendering on/off
        bool hit_testing;         // Collisions on/off
        int frame;

        /* Associated data --------------------------------*/
        struct sm_t *sm;          // State machine
        struct pos_t *pos;        // Position object
        struct list_head inv;     // Inventory object 
        struct equip_t *equip;    // Equipped object
        struct astar_t *astar;    // A* pathfinding object
        uint32_t map_id;          // Current map 

        /* Rendering --------------------------------------*/
        struct ani_t *animation;  // Animation sequence 
        PANEL  *pan;
        WINDOW *win;
        const wchar_t *sprite;
        short   color;

        /* Dynamic methods --------------------------------*/
        enum noun_model model;           // The noun "subclass"
        int  (*_modify)(void *self);     // Master FSM 
        void (*_render)(void *self);     // Draw noun to screen 

        /* Member methods ---------------------------------*/
        int  (*modify)(void *self);
        void (*render)(void *self);
        void (*update)(void *self);
        void (*step) (void *self, int dir);
        void (*fall) (void *self);
        void (*hit)  (void *self);
        void (*seek) (void *self, void *target);
        void (*setyx)(void *self, int y, int x);
        void (*mobile)(void *self, bool opt);
        void (*animate)(void *self, void *animation);
        void (*del)(void *self);
        void (*doom)(void *self);
        void (*player)(void *self, bool opt);
        void (*eat)(void *self);
        void (*pickup)(void *self);
};



/* Public functions 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *new_noun(const char *name, uint32_t model);
void spawn_noun(uint32_t model, int y, int x, bool mobile);
struct noun_t *key_noun(uint32_t id);
struct noun_t *find_noun(uint32_t id);
struct noun_t *get_noun(const char *name);
struct noun_t *get_noun_at(struct map_t *map, int y, int x);
struct noun_t *get_player(void);
int noun_model(uint32_t id);

void free_nouns(void);
void update_nouns(void);

void noun_set_signal_delayed(struct noun_t *noun, uint32_t state, int dir);
void noun_set_signal(struct noun_t *noun, uint32_t state, int dir);
void emit_to_noun(struct noun_t *noun, int dir, uint32_t state);

void noun_set_player(struct noun_t *noun);

struct noun_t *PLAYER;



/* External pointer for queried nouns
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *focused;



#define NOUN(ptr) (struct noun_t *)(ptr)
#define nn(name) get_noun((name))



/* Reference counting
``````````````````````````````````````````````````````````````````````````````*/
struct list_head keyring;  /* Contains all noun keys */

#define NOUN_MAN        0x01000000
#define NOUN_HOPPER     0x02000000

#define NONOUN   0 
#define NOMODEL -1



#endif


