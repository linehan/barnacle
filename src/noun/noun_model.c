/*
 * noun_model.c -- provides an interface for the noun database
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/stoc/stoc.h"
#include "../lib/hash.h"
#include "../lib/textutils.h"
#include "../lib/redblack/rb.h"

#include "noun_model.h"


#define MAXNOUN 1000


struct rb_tree *nountree; /* Holds all registered nouns */
int numnoun;              /* Counts all registered nouns */

/*
 * check_nountree -- used to ensure the tree is initialized and allocated
 */
inline bool check_nountree(void)
{
        if (!nountree) {
                nountree = rb_new(1);
                numnoun  = 0;
        }
        return (nountree) ? true : false;
}

/*
 * add_to_nountree -- insert a noun into the nountree and refcount
 */
inline int add_to_nountree(struct noun_t *noun)
{
        rb_store(nountree, noun->key, noun); 
        keyring[numnoun] = noun->key;

        return numnoun++;
}




/*
 * new_noun -- Create a new noun object and store it in the nountree
 * @name: the name of the noun
 * @model: the type enum of the noun
 * @job: the job or subtype enum of the noun
 * @obj: the class struct containing type-specific data
 */
struct noun_t *new_noun(const char *name, uint32_t model, void *obj)
{
        assert(check_nountree() || "Could not allocate noun tree");

        struct noun_t *new;        
        new = malloc(sizeof(struct noun_t));

        new->name  = mydup(name);
        new->key   = fasthash(name, strlen(name));
        new->model = model;
        new->obj   = obj;

        apply_noun_model(new);
        add_to_nountree(new);

        return new;
}




void noun_set_mob(struct noun_t *noun, bool yesno)
{
        set_mob(&noun->mob, yesno);
        if (yesno == true)
                noun->options |= NOUN_MOBILE;
        else
                noun->options &= ~NOUN_MOBILE;
}





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


/*
 * Load some test data into the nountree
 */
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
                new_noun(name[i], PERSON, 0);
        }

        for (i=0; i<numnoun; i++) {
                set_vital(keyring[i], HP, roll_fair(8));
                set_vital(keyring[i], SP, roll_fair(8));
                set_vital(keyring[i], LP, roll_fair(8));
                set_vital(keyring[i], EP, roll_fair(8));
                key_noun(keyring[i]);
                /*focused->verb.fund = 0x80000000>>vit_blocklen(keyring[i]);*/
        }
}

