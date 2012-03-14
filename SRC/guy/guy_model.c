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
//  |        ;_,._|_,  `, ' \                    guy_model.c                  //
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
#include <stdio.h>
#include <stdlib.h>
#include "../lib/hash.h"
#include "guy_model.h"

struct rb_tree *GUY_TREE; // Holds all creatures

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                helpers                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void focus(uint32_t key)
{
        focused = (struct guy_t *)rb_extra(GUY_TREE, key);
}


int nguys(void)
{
        if (GUY_TREE == NULL)   return 0;
        else                    return (GUY_TREE->n);
}


uint32_t three_name_hash(char *a, char *b, char *c)
{
        uint32_t hashval;
        size_t size;
        char *abc;

        size = strlen(a) + strlen(b) + strlen(c) + 3;
        abc  = malloc(size);

        sprintf(abc, "%s %s %s", a, b, c);
        hashval = fasthash(abc, size);
        free(abc);

        return(hashval);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                new_guy                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
uint32_t new_guy(char *firname,
                 char *midname,
                 char *lasname,
                 char *birthpl,
                 uint8_t job,
                 uint8_t age,
                 uint8_t weight,
                 uint8_t height,
                 uint8_t gender)
{
        if (GUY_TREE == NULL) GUY_TREE = new_tree(1);

        uint32_t key = three_name_hash(firname, midname, lasname);
        struct guy_t *new = malloc(sizeof(struct guy_t));

        size_t fsize = strlen(firname);
        size_t msize = strlen(midname);
        size_t lsize = strlen(lasname);
        size_t bsize = strlen(birthpl);

        memcpy(new->fname, firname, fsize);
        memcpy(new->mname, midname, msize);
        memcpy(new->lname, lasname, lsize);
        memcpy(new->birth, birthpl, bsize);

        new->fname[fsize] = '\0';
        new->mname[msize] = '\0';
        new->lname[lsize] = '\0';
        new->birth[bsize] = '\0';

        new->job    = (uint8_t)job;
        new->age    = (uint8_t)age;
        new->weight = (uint8_t)weight;
        new->height = (uint8_t)height;
        new->gender = (uint8_t)gender;

        new->action = 0;

        keyring[GUY_TREE->n] = key; /* Store the key in the persons array. */

        rb_store(GUY_TREE, key, new);

        return (GUY_TREE->n);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                test                                        //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void load_guy_test(void)
{
        #include "../lib/fyshuffle.h"

        char *fn[24] = { "Robert", 
                         "Jonathan",
                         "Morgan",
                         "Mackenzie",
                         "Matt",
                         "Willard",
                         "Maximilian",
                         "John",
                         "Garth",
                         "Kathryn",
                         "James",
                         "Geordi",
                         "Owen",
                         "Jean-Luc",
                         "Christopher",
                         "Erik",
                         "William",
                         "Richard",
                         "Montgomery",
                         "Tryla",
                         "Benjamin",
                         "Hikaru",
                         "Clark",
                         "Robert"};

        char *mn[24] = { "John",
                         "George",
                         "Rob",
                         "Bob",
                         "Bobnotrob",
                         "Hackensack",
                         "O",
                         "Lingo",
                         "Noam",
                         "Nome",
                         "Foam",
                         "Steele",
                         "Eisen",
                         "Frog",
                         "Cartouche",
                         "Lamy",
                         "Bootstrap",
                         "Wrede",
                         "Avec",
                         "Yank",
                         "Swappy",
                         "Dijk",
                         "Faiyum",
                         "Bergamot"};

        char *ln[24] = { "Aruga",
                         "Ahab",
                         "Cook",
                         "Bligh",
                         "Dana",
                         "Englehorn",
                         "Gault",
                         "Haddock",
                         "Hook",
                         "Hornblower",
                         "April",
                         "Perry",
                         "Jat",
                         "Maak",
                         "Pickle",
                         "Pugwash",
                         "Picard",
                         "Stubing",
                         "Nelson",
                         "Aubrey",
                         "Wilkes",
                         "Bering",
                         "Truxton",
                         "Sakuma"};
        
        char *bp[24] = { "Aruba",
                         "Janjaweed",
                         "Lisbon",
                         "Pickledown",
                         "Idaho",
                         "Lingoville",
                         "Ripfeather",
                         "Penburken",
                         "Delft",
                         "Somewhere",
                         "Willydill",
                         "Bingotown",
                         "Housetown",
                         "Gorlax",
                         "Billings",
                         "Adj",
                         "Fffff",
                         "Howhowhow",
                         "Delicious",
                         "Fork",
                         "Bongomont",
                         "Meatslope",
                         "Ducat",
                         "Guinea"};

        uint8_t j[] = {5,3,1,0,9,2,3,4,5,8,12,3,5,7,5,1,2,2,6,8,22,4,0,23};
        uint8_t g[] = {0};
        uint8_t a[24], w[24], h[24];

        fy_shuffle(j, a, 24);
        fy_shuffle(a, w, 24);
        fy_shuffle(w, h, 24);

        int i;
        for (i=0; i<24; i++) {
                new_guy(fn[i], mn[i], ln[i], bp[i], j[i], a[i], w[i], h[i], g[i]);
        }
        for (i=0; i<nguys(); i++) {
                set_vital(keyring[i], HP, roll_fair(8));
                set_vital(keyring[i], SP, roll_fair(8));
                set_vital(keyring[i], LP, roll_fair(8));
                set_vital(keyring[i], EP, roll_fair(8));
                focus(keyring[i]);
                focused->forward = true;
                focused->fundamental = 0x80000000>>vit_blocklen(keyring[i]);
        }
}


