#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include "../com/arawak.h"
#include "../verb/verb.h"
#include "../mob/mob.h"
#include "../mob/inventory.h"
#include "../map/map.h"
#include "../verb/fsm.h"
#include "models.h"

enum noun_model { NOUN_CREATURE, NOUN_DUMMY };

/* The noun type
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t {
        /* State ------------------------------------------*/
        char    *name;            // String identifier
        uint32_t id;              // Hashed identifier
        uint32_t vitals;          // The state word
        bool is_active;           // Rendering on/off
        bool hit_testing;         // Collisions on/off

        /* Associated data --------------------------------*/
        struct sm_t *sm;          // State machine
        struct pos_t *pos;        // Position object
        struct inventory_t *inv;  // Inventory object 
        struct astar_t *astar;    // A* pathfinding object
        uint32_t map_id;          // Current map 
        void *obj;                // Misc.

        /* Rendering --------------------------------------*/
        struct ani_t *animation;  // Animation sequence 
        PANEL  *pan;
        WINDOW *win;

        /* Dynamic methods --------------------------------*/
        enum noun_model model;    // The noun "subclass"
        MODIFY_METHOD modify;     // Master FSM 
        RENDER_METHOD render;     // Draw noun to screen 

        /* Static methods ---------------------------------*/
        void (*step) (void *self, int dir);
        void (*hit)  (void *self);
        void (*fall) (void *self);
        void (*seek) (void *self, void *target);
        void (*setyx)(void *self, int y, int x);
};



/* Public functions 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *new_noun(const char *name, uint32_t model, void *obj);
void noun_set_render(struct noun_t *noun, RENDER_METHOD func);
void noun_set_modify(struct noun_t *noun, MODIFY_METHOD func);
void noun_set_state(struct noun_t *noun, int state, int value);
void noun_set_mobile(struct noun_t *noun, bool yesno);
void set_animation(struct noun_t *noun, struct ani_t *ani);


struct noun_t *key_noun(uint32_t id);
struct noun_t *get_noun(const char *name);
#define nn(name) (get_noun((name)))
struct noun_t *get_noun_at(struct map_t *map, int y, int x);
void noun_set_signal(struct noun_t *noun, enum sm_state verb, int dir);




/* External pointer for queried nouns
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *focused;




/* Accessor functions 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * noun_obj -- returns a pointer to the obj member of the noun struct
 * @name: name of noun (string)
 */
static inline void *noun_obj(const char *name)
{
        return (void *)(get_noun(name))->obj;
}

/**
 * noun_render -- call the noun's render method
 * @noun: pointer to a struct noun_t
 */
static inline void noun_render(struct noun_t *noun)
{
        noun->render(noun);
}

/**
 * noun_modify -- call the noun's modify method
 * @noun: pointer to a struct noun_t
 */
static inline void noun_modify(struct noun_t *noun)
{
        noun->modify(noun);
}

/**
 * noun_move -- call the noun's step method
 * @noun: pointer to a struct noun_t
 * @dir: direction of step 
 */
static inline void noun_move(struct noun_t *noun, int dir)
{
        noun->step(noun, dir);
}

/**
 * noun_update -- called by the event loop each game tick
 * @noun: pointer to a noun
 */
static inline void noun_update(struct noun_t *noun)
{
        noun_modify(noun);
        noun_render(noun);
}



/* Reference counting
``````````````````````````````````````````````````````````````````````````````*/
extern int numnoun;     /* Number of currently registered nouns */
uint32_t keyring[100];  /* Contains all noun keys */

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
#include "deps/attributes.h"
#include "deps/jobs.h"
#include "deps/names.h"
#include "deps/vitals.h"




#endif


