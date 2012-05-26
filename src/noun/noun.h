#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include "../com/arawak.h"
#include "../map/map.h"
#include "../lib/fsm/fsm.h"
#include "../ai/astar.h"
#include "models.h"
#include "stats.h"
#include "combat.h"


/* The noun type
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t {
        /* State ------------------------------------------*/
        char    *name;            // String identifier
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
        void *obj;                // Misc.

        /* Rendering --------------------------------------*/
        struct ani_t *animation;  // Animation sequence 
        PANEL  *pan;
        WINDOW *win;

        /* Dynamic methods --------------------------------*/
        enum noun_model model;           // The noun "subclass"
        int  (*_modify)(void *self);     // Master FSM 
        void (*_render)(void *self);     // Draw noun to screen 

        /* Static methods ---------------------------------*/
        void (*_mobile)(void *self, bool opt);
        void (*_step) (void *self, int dir);
        void (*_hit)  (void *self);
        void (*_fall) (void *self);
        void (*_seek) (void *self, void *target);
        void (*_setyx)(void *self, int y, int x);
        void (*_update)(void *self);
        void (*_animate)(void *self, void *animation);
        void (*_del)(void *self);
        void (*_take)(void *self, int y, int x);
        void (*_doom)(void *self);
        void (*_player)(void *self, bool opt);

        /* Member methods ---------------------------------*/
        int  (*modify)(void);
        void (*render)(void);
        void (*update)(void);
        void (*step) (int dir);
        void (*fall) (void);
        void (*hit)  (void);
        void (*seek) (void *target);
        void (*setyx)(int y, int x);
        void (*mobile)(bool opt);
        void (*animate)(void *animation);
        void (*del)(void);
        void (*take)(int y, int x);
        void (*doom)(void);
        void (*player)(bool opt);
};



/* Public functions 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *new_noun(const char *name, uint32_t model, void *obj);
struct noun_t *key_noun(uint32_t id);
struct noun_t *get_noun(const char *name);
struct noun_t *get_noun_at(struct map_t *map, int y, int x);
struct noun_t *get_player(void);

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

enum install_key_opts {SUBJECT, OBJECT};
void     install_id(uint32_t id, int option); /* Make a noun the subj/obj */
uint32_t request_id(int option); /* Request the current subj/obj */


#define NOUN_CREATURE 0x00000001
#define NOUN_SAILBOAT 0x00000002
#define NOUN_RENDERED 0x00000004
#define NOUN_MOBILE   0x00000008

#define NOUN_DOSORT 1
#define NOUN_NOSORT 0


/* Support modules 
``````````````````````````````````````````````````````````````````````````````*/
//#include "deps/attributes.h"
//#include "deps/jobs.h"
//#include "deps/names.h"
//#include "deps/vitals.h"




#endif


