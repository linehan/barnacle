// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib/redblack/rb.h"
#include "../eng/state.h"
#include "../lib/hash.h"
#include "../pan/test.h"
#include "pc.h"

inline void init_pc(void)
{
        PC_TREE = new_tree(1);
}

/*
  Accepts a trio of strings, concatenates them, and produces a hash value.
*/
uint32_t three_name_hash(char *a, char *b, char *c)
{
        size_t size;
        char *abc;
        uint32_t hashval;

        size = strlen(a) + strlen(b) + strlen(c) + 3;
        abc  = malloc(size);

        sprintf(abc, "%s %s %s", a, b, c);
        hashval = fasthash(abc, size);
        free(abc);

        return(hashval);
}

/*
  Creates a new player-character and stores it in PC_TREE.
*/
uint32_t new_pc(char *firname,
                char *midname,
                char *lasname,
                char *birthpl,
                uint8_t job,
                uint8_t age,
                uint8_t weight,
                uint8_t height,
                uint8_t gender)
{
        if (PC_TREE == NULL) init_pc();

        uint32_t key = three_name_hash(firname, midname, lasname);
        struct character_t *new = malloc(sizeof(struct character_t));

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

        person_list[PC_TREE->n] = key; /* Store the key in the persons array. */

        rb_store(PC_TREE, key, new);

        return (PC_TREE->n);
}


void attr_all(int *out, uint32_t key)
{
        uint32_t attrcopy;
        int i;

        rb_peek(PC_TREE, key);
        if (PC_TREE->peek != NULL) 
                attrcopy = (uint32_t)PC_TREE->peek->data;

        for (i=0; i<NATTRS; i++) {
                out[i] = see_state(i, &attrcopy);
        }
}



void load_pc_test(void)
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
                new_pc(fn[i], mn[i], ln[i], bp[i], j[i], a[i], w[i], h[i], g[i]);
        }
}
