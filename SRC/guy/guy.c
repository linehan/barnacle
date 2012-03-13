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
#include "guy.h"
#include "vit.h"
#include "jobs.h"

inline void init_guy(void)
{
        GUY_TREE = new_tree(1);
}

/*
  Accepts a trio of strings, concatenates them, and produces a hash value.
*/
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

/*
  Creates a new player-character and stores it in GUY_TREE.
*/
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
        if (GUY_TREE == NULL) init_guy();

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

        muster[GUY_TREE->n] = key; /* Store the key in the persons array. */

        rb_store(GUY_TREE, key, new);

        return (GUY_TREE->n);
}


void attr_pkg(int *out, uint32_t key)
{
        uint32_t attrcopy;
        int i;

        rb_peek(GUY_TREE, key);
        if (GUY_TREE->peek != NULL) 
                attrcopy = (uint32_t)GUY_TREE->peek->data;

        for (i=0; i<8; i++) {
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
                new_guy(fn[i], mn[i], ln[i], bp[i], j[i], a[i], w[i], h[i], g[i]);
        }
}



/* Return character stat n
------------------------------------------------------------------------------*/
uint32_t get_cstat(uint32_t key, int n)
{
        return (get_state(GUY_TREE, key, 0, n));
}
/* Set character stat n to value v
------------------------------------------------------------------------------*/
void set_cstat(uint32_t key, int n, int v)
{
        set_state(GUY_TREE, key, 0, n, v);
}
/* Return job string
------------------------------------------------------------------------------*/
char *getjob(uint32_t key)
{
        focus(key);
        return (job_list[(int)focused->job]);
}
/* Returns first name
------------------------------------------------------------------------------*/
char *fname(uint32_t key)
{
        focus(key);
        return (focused->fname);
}
/* Returns middle name
------------------------------------------------------------------------------*/
char *mname(uint32_t key)
{
        focus(key);
        return (focused->mname);
}
/* Returns last name
------------------------------------------------------------------------------*/
char *lname(uint32_t key)
{
        focus(key);
        return (focused->lname);
}
/* Returns first and last name
------------------------------------------------------------------------------*/
char *flname(uint32_t key)
{
        char *f, *l;

        f = fname(key);
        l = lname(key);

        char *fl = malloc(strlen(f) + strlen(l) + 2);

        sprintf(fl, "%s %s", f, l);

        return (fl);
}
/* Returns full name (just a tweaked three_name_hash)
------------------------------------------------------------------------------*/
char *fullname(uint32_t key)
{
        char *a, *b, *c;

        a = fname(key);
        b = mname(key);
        c = lname(key);

        char *abc = malloc(strlen(a) + strlen(b) + strlen(c) + 3);

        sprintf(abc, "%s %s %s", a, b, c);

        return (abc);
}

uint32_t *vit_getpkg(uint32_t key)
{
        focus(key);
        return (&focused->vitals);
}

/* Return the number of creatures currently loaded
------------------------------------------------------------------------------*/
int npersons(void)
{
        if (GUY_TREE == NULL)   return 0;
        else                    return (GUY_TREE->n);
}
