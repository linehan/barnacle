// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include "itm.h"

#define NKINDS 128                // Number of kinds == max unsigned char
struct dom_t *DOMAIN_FOR[NKINDS]; // Each kind has a domain.

//##############################################################################
// Store a struct itm_t (and its record) in the red-black tree contained 
// in the domain matching the item's kind.
//##############################################################################
#define store_itm(item) \
        rb_store(DOMAIN_FOR[item->id->kind]->tree, item->id->key, item)
//##############################################################################
// Remove a struct itm_t from the red-black tree of which it is a member.
//##############################################################################
#define remove_itm(item) \
        rb_remove(DOMAIN_FOR[item->id->kind]->tree, item->id->key, item)
//##############################################################################
//  Register a domain for a particular kind, so that it can be associated
//  later with objects of that kind which are registered using register_itm().  
//##############################################################################
struct dom_t *register_dom(unsigned char kind, TOC nibbles, GLOSS options)
{
        if (DOMAIN_FOR[kind] != NULL) // If a domitch already exists.
                return (0);
        else {
                struct dom_t *new = new_dom(nibbles, options);
                DOMAIN_FOR[kind] = new;
                return (DOMAIN_FOR[kind]); // The source file gets its own copy.
        }
}
//##############################################################################
// Prepare a struct itm_t to house the record stored at *data. Then, check 
// the item into the appropriate domain.
//##############################################################################
uint32_t register_itm(unsigned char kind, void *data)
{
        if (DOMAIN_FOR[kind] == NULL) // If the domitch is not initialized.
                return (0);
        else {
                struct itm_t *new = malloc(sizeof(struct itm_t));

                new->id = new_id(kind);
                new->dom = DOMAIN_FOR[kind];
                new->data = data;
                store_itm(new);

                return (new->id->key); // Return the key to the new item.
        }
}
//##############################################################################
// Return a pointer to the item of kind 'kind' with key 'key'.
//##############################################################################
struct itm_t *retreive_itm(unsigned char kind, uint32_t key)
{
        rb_peek(DOMAIN_FOR[kind]->tree, key);
        return (struct itm_t *)DOMAIN_FOR[kind]->tree->peek->box;
}
