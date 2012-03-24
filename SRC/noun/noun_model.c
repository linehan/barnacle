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
#include "../lib/hash.h"
#include "noun_model.h"
#include "../lib/textutils.h"

#define MAXNOUN 1000

struct rb_tree *NOUNTREE;
int NNOUNS;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                helpers                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void focus(uint32_t key)
{
        struct noun_t *temp = (struct noun_t *)rb_extra(NOUNTREE, key);
        assert (temp != NULL);
        focused = temp;
}


int nnouns(void)
{
        return NNOUNS;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                new_noun                                    //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void new_noun(char *name, uint8_t job)
{
        struct noun_t *new = malloc(sizeof(struct noun_t));
        uint32_t key;

        new->name = mydup(name);
        new->job = (uint8_t)job;

        new->verb.empty = true;
        new->verb.forw  = true;

        new->verb.give = FIFO_INIT;
        new->verb.get  = FIFO_INIT;
        new->verb.from = FIFO_INIT;
        new->verb.to   = FIFO_INIT;

        key = fasthash(name, strlen(name));

        if (NOUNTREE == NULL) {
                NOUNTREE = new_tree(1);
                NNOUNS = 0;
        }

        rb_store(NOUNTREE, key, new);

        keyring[NNOUNS] = key;
        NNOUNS++;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  keys                                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
static int loaded;
uint32_t active_key[2];

void install_key(uint32_t key, int option)
{
        if (option!=OBJECT && option!=SUBJECT) return;
        else
                active_key[option] = key;
}


uint32_t request_key(int option)
{
        if (!loaded) {
                active_key[0] = keyring[0];
                active_key[1] = keyring[0];
                loaded = 1;
        }
        if (option!=OBJECT && option!=SUBJECT) return active_key[0];
        else
                return (active_key[option]);
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

        char *name[24] = { "Robert Aruga", 
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

        uint8_t j[] = {5,3,1,0,9,2,3,4,5,8,12,3,5,7,5,1,2,2,6,8,22,4,0,23};

        int i;
        for (i=0; i<24; i++) {
                new_noun(name[i], j[i]); 
        }


        for (i=0; i<nnouns(); i++) {
                set_vital(keyring[i], HP, roll_fair(8));
                set_vital(keyring[i], SP, roll_fair(8));
                set_vital(keyring[i], LP, roll_fair(8));
                set_vital(keyring[i], EP, roll_fair(8));
                focus(keyring[i]);
                focused->verb.fund = 0x80000000>>vit_blocklen(keyring[i]);
        }
}

