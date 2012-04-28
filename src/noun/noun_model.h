#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include <stdbool.h>
#include <stdint.h>
#include "../verb/verb.h"
#include "../eng/bytes.h"
#include "../lib/redblack/rb.h"
#include "../lib/hash.h"
#include "../mob/mob.h"


enum nountypes {
        SAILBOAT, 
        PERSON, 
        MONSTER,
        DUMMY,
};


/* The noun data structure 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t {
        char    *name;          /* String identifier */
        uint32_t key;           /* Hashed identifier */
        uint32_t model;         /* Polymorph. model */
        uint32_t vitals;        /* The state word */
        uint32_t attributes;    /* The options word */
        uint32_t options;       /* Used by the query system */
        uint32_t state;         /* for the AI */
        struct verb_t verb;     /* A verb package for messaging (verb.c) */
        struct mob_t mob;       /* A mob package for rendering (mob.c) */
        void *obj;              /* A private data type */
        void (*render)(void *self);
        int  (*modify)(void *self, int verb, int value);
};


/* Functions 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_t *new_noun(const char *name, uint32_t model, void *obj);
void load_noun_test(void);

struct rb_tree *nountree;
struct noun_t *focused;


/* Query functions 
``````````````````````````````````````````````````````````````````````````````*/
static inline 
bool noun_exists(uint32_t key)
{
        return (rb_extra(nountree, key)) ? true : false;
}

/*
 * key_noun -- given a noun's id, set the 'focused noun' pointer to that noun
 * @id: unsigned 32-bit unique id
 */
static inline
struct noun_t *key_noun(uint32_t id)
{
        struct noun_t *tmp;
        tmp = rb_extra(nountree, id);
        //assert(tmp != NULL);
        focused = tmp;
        return (focused);
}

/*
 * get_noun -- given a name string, return a pointer to the noun
 * @name: name of the noun to be returned
 */
static inline
struct noun_t *get_noun(const char *name)
{
        key_noun(fasthash(name, strlen(name)));
        return (focused);
}

/*
 * get_noun_at -- given coordinates y, x, return any noun at that position
 * @y: y-coordinate
 * @x: x-coordinate
 */
static inline
struct noun_t *get_noun_at(struct map_t *map, int y, int x)
{
        return (key_noun(mx_val(map->mobs, y, x)));  
}


/* Accessor functions 
``````````````````````````````````````````````````````````````````````````````*/
/*
 * noun->obj -- returns a pointer to the obj member of the noun struct
 * @name: name of noun (string)
 */
static inline 
void *noun_obj(const char *name)
{
        return (void *)(get_noun(name))->obj;
}

/*
 * &noun->mob -- returns a pointer to the mob member of the noun struct
 * @name: name of noun (string)
 */
static inline 
struct mob_t *noun_mob(const char *name)
{
        return &((get_noun(name))->mob);
}

/*
 * noun_set_render -- set the render method of the noun
 * @noun: pointer to a struct noun_t
 * @func: pointer to a function returning void and accepting void *
 */
static inline 
void noun_set_render(struct noun_t *noun, void (*func)(void *obj))
{
        noun->render = func;
}

/*
 * noun_set_modify -- set the modify method of the noun
 * @noun: pointer to a struct noun_t
 * @func: pointer to a function returning int and accepting void * and int
 */
static inline
void noun_set_modify(struct noun_t *noun, int (*func)(void *obj, int verb, int value))
{
        noun->modify = func;
}

/*
 * noun_render -- call the noun's render method
 * @noun: pointer to a struct noun_t
 */
static inline
void noun_render(struct noun_t *noun)
{
        noun->render(noun);
}

/*
 * noun_modify -- call the noun's modify method
 * @noun: pointer to a struct noun_t
 * @opt: value to be interpreted by the modify method
 */
static inline
void noun_modify(struct noun_t *noun, int verb, int value)
{
        noun->modify(noun, verb, value);
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

