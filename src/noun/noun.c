/*
 * noun_model.c -- provides an interface for the noun database
 */
#include <assert.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/hash.h"
#include "../lib/stoc/stoc.h"
#include "../lib/textutils.h"
#include "../lib/redblack/rb.h"
#include "noun.h"


#define MAXNOUN 1000


struct rb_tree *nountree; /* Holds all registered nouns */
int numnoun;              /* Counts all registered nouns */


/* Helper functions
``````````````````````````````````````````````````````````````````````````````*/
/**
 * check_nountree (PRIVATE) -- ensure the tree is initialized and allocated
 */
inline bool check_nountree(void)
{
        if (!nountree) {
                nountree = rb_new(1);
                numnoun  = 0;
        }
        return (nountree) ? true : false;
}

/**
 * add_to_nountree (PRIVATE) -- insert a noun into the nountree and refcount
 */
inline int add_to_nountree(struct noun_t *noun)
{
        rb_store(nountree, noun->key, noun); 
        keyring[numnoun] = noun->key;

        return numnoun++;
}



/* Noun constructor 
``````````````````````````````````````````````````````````````````````````````*/
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



/* Class assignment and dynamic linkages 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * noun_set_render -- set the render method of the noun
 * @noun: pointer to a struct noun_t
 * @func: pointer to a function returning void and accepting void *
 */
void noun_set_render(struct noun_t *noun, RENDER_METHOD func)
{
        noun->render = func;
}

/**
 * noun_set_modify -- set the modify method of the noun
 * @noun: pointer to a struct noun_t
 * @func: pointer to a function returning int and accepting void * and int
 */
void noun_set_modify(struct noun_t *noun, MODIFY_METHOD func)
{
        noun->modify = func;
}

/**
 * noun_set_state -- set the state and optionial state value of noun
 * @noun: pointer to a struct noun_t
 * @state: state to transition into 
 * @value: value of the state (optional)
 */
void noun_set_state(struct noun_t *noun, int state, int value)
{
        noun->state = state;
        noun->value = value;
}

/**
 * noun_set_mob -- define a noun as being on the world map
 * @noun: pointer to a noun
 * @yesno: whether the noun should be added or removed from the world map
 */
void noun_set_mobile(struct noun_t *noun, bool yesno)
{
        set_mob(&noun->mob, yesno);
        if (yesno == true)
                noun->options |= NOUN_MOBILE;
        else
                noun->options &= ~NOUN_MOBILE;
}



/* Queries and retreival 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * key_noun -- given a noun's id, set the 'focused noun' pointer to that noun
 * @id: unsigned 32-bit unique id
 */
struct noun_t *key_noun(uint32_t id)
{
        struct noun_t *tmp;
        tmp = rb_extra(nountree, id);
        //assert(tmp != NULL);
        focused = tmp;
        return (focused);
}

/**
 * get_noun -- given a name string, return a pointer to the noun
 * @name: name of the noun to be returned
 */
struct noun_t *get_noun(const char *name)
{
        key_noun(fasthash(name, strlen(name)));
        return (focused);
}

/**
 * get_noun_at -- given coordinates y, x, return any noun at that position
 * @y: y-coordinate
 * @x: x-coordinate
 */
struct noun_t *get_noun_at(struct map_t *map, int y, int x)
{
        return (key_noun(mx_val(map->mobs, y, x)));  
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

