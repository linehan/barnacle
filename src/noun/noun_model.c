////////////////////////////////////////////////////////////////////////////////
//    |  \ \ | |/ /                                                           //
//    |  |\ `' ' /                                                            //
//    |  ;'      \      / ,                                                   //
//    | ;    _,   |    / / ,                                                  //
//    | |   (  `-.;_,-' '-' ,                                                 //
//    | `,   `-._       _,-'_                                                 //
//    |,-`.    `.)    ,<_,-'_,                                                //
//   ,'    `.   /   ,'  `;-' _,                                               //
//  ;        `./   /`,    \-'                                                 //
//  |         /   |  ;\   |\                                                  //
//  |        ;_,._|_,  `, ' \                    noun_model.c                 //
//  |        \    \ `       `,                                                //
//  `      __ `    \         ;,                                               //
//   \   ,'  `      \,       ;,                                               //
//    \_(            ;,      ;;                                               //
//    |  \           `;,     ;;                                               //
//    |  |`.          `;;,   ;'                                               //
//    |  |  `-.        ;;;;,;'                                                //
//    |  |    |`-.._  ,;;;;;'                                                 //
//    |  |    |   | ``';;;'                                                   //
////////////////////////////////////////////////////////////////////////////////
#define _XOPEN_SOURCE_EXTENDED = 1 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/stoc/stoc.h"
#include "../lib/hash.h"
#include "../lib/textutils.h"
#include "../lib/redblack/rb.h"

#include "noun_model.h"

#define MAXNOUN 1000


struct rb_tree *nountree;
int numnoun;


/*
 * focus -- given an id, set the 'focused' pointer to the matching noun_t
 * @id: the unique identifier of the noun
 */ 
void focus(uint32_t id)
{
        struct noun_t *tmp;

        tmp = rb_extra(nountree, id);
        assert(tmp != NULL);

        focused = tmp;
}



/*
 * new_noun -- create a new noun object and store it in the nountree
 * @name: the name of the noun
 * @type: the type of the noun, such as "person" or "boat"
 * @job: the job or subtype of the noun, such as "monk" or "yawl"
 */
struct noun_t *new_noun(const char *name, uint32_t type, uint32_t job, void *obj)
{
        struct noun_t *new;        
        uint32_t id;

        new = malloc(sizeof(struct noun_t));

        new->name = mydup(name);
        new->type = type;
        new->job  = job;
        new->obj  = obj;

        new->verb.empty = true;
        new->verb.forw  = true;

        new->verb.give = FIFO_INIT;
        new->verb.get  = FIFO_INIT;
        new->verb.from = FIFO_INIT;
        new->verb.to   = FIFO_INIT;

        if (!nountree) {
                nountree = rb_new(1);
                numnoun = 0;
        }

        id = fasthash(new->name, strlen(new->name));

        rb_store(nountree, id, new);
        keyring[numnoun++] = id;

        return new;
}

void *noun_obj(const char *name)
{
        return (void *)(get_noun(name))->obj;
}

struct mob_t *noun_mob(const char *name)
{
        return &((get_noun(name))->mob);
}

void noun_set_render(struct noun_t *noun, void (*func)(void *obj))
{
        noun->render = func;
}
void noun_set_modify(struct noun_t *noun, int (*func)(void *obj, int opt))
{
        noun->modify = func;
}

void noun_render(struct noun_t *noun)
{
        noun->render(noun);
}

void noun_modify(struct noun_t *noun, int opt)
{
        noun->modify(noun, opt);
}



/*
 * get_noun -- given a name string, return a pointer to the noun_t
 * @name: name of the noun to be returned
 */
struct noun_t *get_noun(const char *name)
{
        focus(fasthash(name, strlen(name)));
        return (focused);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  keys                                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
static int loaded;
uint32_t active_id[2];

void install_id(uint32_t id, int option)
{
        if (option!=OBJECT && option!=SUBJECT) return;
        else
                active_id[option] = id;
}


uint32_t request_id(int option)
{
        if (!loaded) {
                active_id[0] = keyring[0];
                active_id[1] = keyring[0];
                loaded = 1;
        }
        if (option!=OBJECT && option!=SUBJECT) return active_id[0];
        else
                return (active_id[option]);
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                test                                        //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void load_noun_test(void)
{

        char *name[24]={ "Robert Aruga", 
                         "Jonathan Ahab",
                         "Morgan Cook",
                         "Mackenzie Bligh",
                         "Matt Dana",
                         "Willard Englehorn",
                         "Maximilian Gault",
                         "John Haddock",
                         "Garth Hook",
                         "Kathryn Hornblower",
                         "James April",
                         "Geordi Perry",
                         "Owen Jat",
                         "Jean-Luc Maak",
                         "Christopher Pickle",
                         "Erik Pugwash",
                         "William Picard",
                         "Richard Stubing",
                         "Montgomery Nelson",
                         "Tryla Aubrey",
                         "Benjamin Wilkes",
                         "Hikaru Bering",
                         "Clark Truxton",
                         "Robert Sakuma"};

        uint32_t j[] = {5,3,1,0,9,2,3,4,5,8,12,3,5,7,5,1,2,2,6,8,22,4,0,23};

        int i;
        for (i=0; i<24; i++) {
                new_person(name[i], j[i]);
        }

        for (i=0; i<numnoun; i++) {
                set_vital(keyring[i], HP, roll_fair(8));
                set_vital(keyring[i], SP, roll_fair(8));
                set_vital(keyring[i], LP, roll_fair(8));
                set_vital(keyring[i], EP, roll_fair(8));
                focus(keyring[i]);
                focused->verb.fund = 0x80000000>>vit_blocklen(keyring[i]);
        }
}

