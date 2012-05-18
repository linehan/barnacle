#ifndef __HASHTABLE_ROUTINES
#define __HASHTABLE_ROUTINES
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "../list.h"
#include "hashfunc.h"

/*
 * MACROS
 */
#define TABLESIZE UINT16_MAX 
#define HASH_LEN 16 
#define HASH(k) (k >> HASH_LEN)



/* ABSTRACT TYPES
``````````````````````````````````````````````````````````````````````````````*/
/* 
 * HASH NODE contains a circular linked list of HASH RECORDS 
 */
struct htab_record { 
        struct list_node node; 
        uint32_t key; 
        void *data; 
};

/* 
 * HASHTABLE
 * The hashtable is an array of hashnodes
 */
struct htab_t {
        struct list_head *tbl[TABLESIZE]; 
        size_t n;
};





/* HASH RECORD FUNCTIONS 
``````````````````````````````````````````````````````````````````````````````*/
/*
 * NEW HTAB RECORD
 * new_htab_record -- create a new record in the hashtable
 */
static inline
struct htab_record *new_htab_record(uint32_t key, void *data)
{
        struct htab_record *new = calloc(1, sizeof(struct htab_record));
        assert(new || !"Could not allocate htab record\n");

        new->key  = key;
        new->data = data;

        return (new);
}


/* 
 * ADD HTAB RECORD
 * add_htab_record -- add a record to the hashtable
 */
static inline
void add_htab_record(struct htab_t *tbl, struct htab_record *record)
{
        assert(tbl    || !"Hash table does not exist");
        assert(record || !"Hash record does not exist");

        uint16_t hashkey = HASH(record->key);

        /* Create a new node at index 'hashkey' if none exists */
        if (!tbl->tbl[hashkey]) {
                tbl->tbl[hashkey] = calloc(1, sizeof(struct list_head));
                assert(tbl->tbl[hashkey] || !"Could not allocate htab head");
                list_head_init(tbl->tbl[hashkey]);
        }
        /* Add the record to the chain at the hash node */
        list_add(tbl->tbl[hashkey], &record->node);
        tbl->n++; /* Increment the table's record count */
}



/*
 * GET HTAB RECORD
 * get_htab_record -- get a record from the hashtable
 */
static inline 
struct htab_record *get_htab_record(struct htab_t *tbl, uint32_t key)
{
        assert(tbl || !"Hash table does not exist");

        struct htab_record *tmp;
        uint16_t hashkey;

        hashkey = HASH(key);

        if (!tbl->tbl[hashkey])
                return NULL;

        if (list_empty(tbl->tbl[hashkey]))
                return NULL;

        list_for_each(tbl->tbl[hashkey], tmp, node) {
                if (tmp->key == key)
                        break;
        }
        return (tmp);
}



/*
 * POP HTAB RECORD
 * pop_htab_record -- pop a record from the hashtable
 */
static inline 
struct htab_record *pop_htab_record(struct htab_t *tbl, uint32_t key)
{
        assert(tbl || !"Hash table does not exist");

        struct htab_record *record;
        uint16_t hashkey;

        hashkey = HASH(key);

        /* Retreive record from hash table */
        if (record = get_htab_record(tbl, key), !record)
                return NULL;

        /* Delete record from list */
        list_del_from(tbl->tbl[hashkey], &record->node);
        tbl->n--;

        return (record);
}



/*
 * DEL HTAB RECORD LIST
 * del_htab_record_list -- destroy an entire list of records at an index
 */
static inline
void del_htab_record_list(struct list_head *head) 
{
        assert(head || !"Hash record list does not exist");

        struct htab_record *tmp;
        struct htab_record *nxt;

        list_for_each_safe(head, tmp, nxt, node) {
                list_del(&tmp->node);
                free(tmp);
        }

        assert(list_empty(head) || !"Unable to delete hash record list");

        free(head);
}



/* HASHTABLE FUNCTIONS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * HTAB POP
 * htab_pop -- remove and return a stored value from the hashtable
 */
static inline
void *htab_pop(struct htab_t *tbl, uint32_t key)
{
        assert(tbl || !"Hash table does not exist");
        struct htab_record *record;
        void *data;

        if (record = pop_htab_record(tbl, key), !record);
                return NULL;

        data = record->data;
        free(record);

        return (data);
}


/*
 * HTAB ADD 
 * htab_add -- insert a new key/value record into the hash table 
 */
static inline
void htab_add(struct htab_t *tbl, uint32_t key, void *data)
{
        add_htab_record(tbl, new_htab_record(key, data));
}


/*
 * HTAB GET
 * htab_get -- return a value stored in the hash table
 */
static inline
void *htab_get(struct htab_t *tbl, uint32_t key)
{
        struct htab_record *record;

        if (record = get_htab_record(tbl, key), record)
                return record->data;
        else
                return NULL;
}


/*
 * HTAB EXISTS
 * htab_exists -- return a boolean value (membership test)
 */
static inline
bool htab_exists(struct htab_t *tbl, uint32_t key) 
{
        return (htab_get(tbl, key)) ? true : false;
}


/*
 * HTAB DEL
 * del_htab -- destroy a hash table
 */
static inline 
void del_htab(struct htab_t *tbl)
{
        int i;

        for (i=0; i<TABLESIZE; i++) {
                if (tbl->tbl[i])
                        del_htab_record_list(tbl->tbl[i]);
        }
        free(tbl);
}


/*
 * HTAB NEW
 * new_htab -- create a new hash table
 */        
static inline
struct htab_t *new_htab(int mode)
{
        struct htab_t *new = calloc(1, sizeof(struct htab_t));

        assert(new || !"Could not allocate hash table\n");

        return (new);
}


#endif

