#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include "../com/arawak.h"
#include "../verb/verb.h"
#include "../mob/mob.h"
#include "../mob/inventory.h"
#include "../map/map.h"
#include "models.h"

enum noun_model { NOUN_CREATURE, NOUN_DUMMY };

/* Abstract types 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t {
        /* ----------------------------------------------- Identifiers */
        char    *name;          /* String identifier */
        uint32_t id;            /* Hashed identifier */

        /* ----------------------------------------------- State */
        uint32_t vitals;        /* The state word */
        int state;              /* for the AI */
        int value;              /* For the AI */
        bool is_active;
        bool hit_testing_enabled;

        /* ----------------------------------------------- Messaging */
        struct verb_t verb;     
        uint32_t signal;

        /* ----------------------------------------------- Associated data */
        struct inventory_t *inv;   
        struct astar_t     *astar;
        struct pos_t       *pos;
        uint32_t map_id;
        void *obj;              

        /* ----------------------------------------------- Rendering */
        struct ani_t *animate;
        PANEL  *pan;
        WINDOW *win;

        /* ----------------------------------------------- Dynamic methods */
        enum noun_model model;
        MODIFY_METHOD modify;   /* Dynamic linkage for state machine */
        RENDER_METHOD render;   /* Dynamic linkage for rendering */

        /* ----------------------------------------------- Static methods */
        void (*step)(void *self, int dir);
        void (*setyx)(void *self, int y, int x);
        void (*hit)(void *self);
        void (*fall)(void *self);
        void (*seek)(void *self, void *target);
};



/* Public functions 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *new_noun(const char *name, uint32_t model, void *obj);
void noun_set_render(struct noun_t *noun, RENDER_METHOD func);
void noun_set_modify(struct noun_t *noun, MODIFY_METHOD func);
void noun_set_state(struct noun_t *noun, int state, int value);
void noun_set_mobile(struct noun_t *noun, bool yesno);

struct noun_t *key_noun(uint32_t id);
struct noun_t *get_noun(const char *name);
struct noun_t *get_noun_at(struct map_t *map, int y, int x);

void load_noun_test(void);



/* External pointer for queried nouns
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *focused;




/* Accessor functions 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * noun->obj -- returns a pointer to the obj member of the noun struct
 * @name: name of noun (string)
 */
static inline void *noun_obj(const char *name)
{
        return (void *)(get_noun(name))->obj;
}

/**
 * &noun->pos -- returns a pointer to the mob member of the noun struct
 * @name: name of noun (string)
 */
static inline struct pos_t *noun_pos(const char *name)
{
        return ((get_noun(name))->pos);
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
 * @opt: value to be interpreted by the modify method
 */
static inline void noun_modify(struct noun_t *noun)
{
        noun->modify(noun);
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


static inline PANEL *noun_pan(struct noun_t *noun)
{
        return (noun->pan);
}
static inline WINDOW *noun_win(struct noun_t *noun)
{
        return panel_window(noun->pan);
}
static inline void noun_move(struct noun_t *noun, int dir)
{
        noun->step(noun, dir);
}

#endif


