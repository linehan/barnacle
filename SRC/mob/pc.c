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
#include "../kin/k_character.h"
#include "../lib/hash.h"
#include "../pan/test.h"

/* Holds all player characters in the program. */
struct rb_tree *PC_TREE;

#define MAX_PEOPLE 100
uint32_t person_list[MAX_PEOPLE];

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


/* Return character stat n */
uint32_t get_cstat(uint32_t key, int n)
{
        return (get_state(PC_TREE, key, 0, n));
}

/* Set character stat n to value v */
void set_cstat(uint32_t key, int n, int v)
{
        set_state(PC_TREE, key, 0, n, v);
}

/* Increment character stat n by 1 */
void inc_cstat(uint32_t key, int n)
{
        set_state(PC_TREE, key, 0, n, get_cstat(key, n)+1);
}

/* Decrement character stat n by 1 */
void dec_cstat(uint32_t key, int n)
{
        set_state(PC_TREE, key, 0, n, get_cstat(key, n)-1);
}

/* Returns first name. */
char *fname(uint32_t key)
{
        struct character_t *guy;
        guy = (struct character_t *)rb_extra(PC_TREE, key);

        return (guy->fname);
}

/* Returns middle name. */
char *mname(uint32_t key)
{
        struct character_t *guy;
        guy = (struct character_t *)rb_extra(PC_TREE, key);

        return (guy->mname);
}

/* Returns last name. */
char *lname(uint32_t key)
{
        struct character_t *guy;
        guy = (struct character_t *)rb_extra(PC_TREE, key);

        return (guy->lname);
}

/* Returns full name (just a tweaked three_name_hash). */
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

int npersons(void)
{
        if (PC_TREE == NULL)    return 0;
        else                    return (PC_TREE->n);
}
