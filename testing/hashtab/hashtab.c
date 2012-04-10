#include <stdlib.h>
#include <stdio.h>

#include "list.h"


struct tabnode_t {
        void *key;
        struct list_node node;
};
 
struct htab_t {
        struct list_head *table;
        size_t size;
};
 

/*
 * htab_get -- retreive an item from the hashtable
 * @tab: pointer to the struct htab_t
 * @key: the key to be hashed 
 * @size: the size of the key
 */
int htab_get(struct htab_t *table, void *key, size_t size)
{
        struct tabnode_t *match;
        size_t h;


        /* Get an index for the key */
        h = hash(key, size) % table->size;
 
        if (table->table[h] != EMPTY) {
                /* Find the matching key in the chain, if any */
                match = table->table[h];
         
                while (match != NULL) {
                        if (compare(key, it->key) == 0)
                                return 1;
                        it = it->next;
                }
        }
        return 0;
}


/*
 * htab_ins -- insert a new item into the hash table
 * @tab: pointer to the struct htab_t
 * @key: the key to be hashed
 * @size: the size of the key
 * @allow_dup: whether duplicates (collisions) should be allowed
 */
int htab_ins(struct htab_t *table, void *key, size_t size, int allow_dup)
{
        struct tabnode_t *match;
        size_t h;
 
        /* Get an index for the key */
        h = hash (key, size) % table->size;
 
        if (!allow_duplicates) {
                /* Search for any duplicate keys */
                it = table->table[h];
         
                while ( it != NULL ) {
                        /* Stop the insertion if a duplicate is found */
                        if (compare(key, it->key) == 0) 
                                return 0;
                        it = it->next;
                }
        }
        /* Create a new node for the chain */
        it = malloc(sizeof *it);
 
        if (it == NULL)
                return 0;
 
        /* Attach the new node to the front of the chain */
        it->key = key;
        it->next = table->table[h];
        table->table[h] = it;
 
        return 1;
}



/*
 * htab_rem -- remove an item from the hash table
 * @tab: pointer to the struct htab_t
 * @key: the key to be hashed
 * @size: the size of the key
 * @allow_dup: whether duplicates (collisions) should be allowed
 */
int htab_rem(struct htab_t *table, void *key, size_t size, int allow_dup)
{
        size_t h;
        int item_found;
 
        item_found = 0;
        /* Get an index for the key */
        h = hash ( key, size ) % table->size;
 
        /* The chain is empty, no more work needed */
        if (table->table[h] == NULL)
                return 0;
 
        /* Always try to remove the first occurrence */
        item_found = remove_node (&table->table[h], key);
 
        if (item_found && allow_duplicates) {
                /* Try to remove all subsequent occurrences */
                while (remove_node(&table->table[h], key));
                /* At least one was found if we got here */
                item_found = true;
        }
        return item_found;
}


