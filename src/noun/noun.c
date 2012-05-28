/*
 * noun_model.c -- provides an interface for the noun database
 */
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../lib/textutils.h"
#include "../lib/hash/hash.h"
#include "../item/item.h"
#include "../lib/fsm/fsm.h"
#include "noun.h"


struct noun_t *PLAYER;


/* NOUN STORAGE 
``````````````````````````````````````````````````````````````````````````````*/
struct noun_key { struct list_node node; uint32_t key; };
/*
 * SEQUENTIAL ACCESS
 * Active noun keys are cached in a circular linked list called 'keyring'.
 * This arrangement is useful for iterating over every noun. Each node of
 * 'keyring' has type struct noun_key, defined below.
 */
LIST_HEAD(keyring);
/*
 * DOOM RING
 * Nouns scheduled for deletion will be culled at the beginning of each
 * tick, before any processing on them would be done. See the doom() function.
 */
LIST_HEAD(doomring);


/*
 * RANDOM ACCESS
 * Nouns are stored and queried from a hashtable which hashes the name of
 * each noun (a string) to generate a key. Accessor functions mediate
 * interaction with 'nountable'.
 */
struct htab_t *nountable;




/* NOUN STORAGE FUNCTIONS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * CHECK 
 * check_nountable -- ensure the hashtable is initialized and allocated */ 
inline void check_nountable(void)
{
        if (!nountable) { 
                nountable = new_htab(0);
        }
        assert(nountable || "Could not allocate noun table");
}

/**
 * HASH
 * hash -- the hash function for noun name strings */
static inline uint32_t hash(const char *name)
{
        return (fasthash(name, strlen(name)));
}

/**
 * STORE
 * add_to_nountable -- insert a noun into the noun hash table and refcount */
void add_to_nountable(struct noun_t *noun)
{
        struct noun_key *new = calloc(1, sizeof(struct noun_key));

        new->key = noun->id;
        list_add(&keyring, &new->node);

        htab_add(nountable, noun->id, noun); 
}

/**
 * DELETE 
 * del_from_nountable -- remove a noun from the noun hash table and refcount */
void del_from_nountable(struct noun_t *noun)
{
        struct noun_key *tmp, *nxt;

        if (list_empty(&keyring))
                return;

        list_for_each_safe(&keyring, tmp, nxt, node) {
                if (tmp->key == noun->id) {
                        list_del_from(&keyring, &tmp->node);
                }
        }

        htab_pop(nountable, noun->id); 
}

/**
 * FREE DOOMED NOUNS
 * free_nouns -- frees all the nouns on the doomring
 */
void free_nouns(void)
{
        struct noun_key *tmp, *nxt;
        struct noun_t *noun;

        if (list_empty(&doomring))
                return;

        list_for_each_safe(&doomring, tmp, nxt, node) {
                list_del_from(&doomring, &tmp->node);
                noun = key_noun(tmp->key);
                noun->_del(noun);
        }
}

/**
 * UPDATE ALL NOUNS
 * Calls the update method of each active noun
 */
void update_nouns(void)
{
        struct noun_key *tmp;
        struct noun_t *noun;

        if (list_empty(&keyring))
                return;

        list_for_each(&keyring, tmp, node) {
                noun = key_noun(tmp->key);
                if (noun != NULL) {
                        noun->_modify(noun);
                        noun->_render(noun);
                }
        }
}




/* NOUN CONSTRUCTOR 
``````````````````````````````````````````````````````````````````````````````*/
/******************************************************************************
 * THE NOUN OBJECT 
 * A noun is a container, an object that holds an aggregate of smaller 
 * objects called 'members'. The noun type is a true object: it contains
 * methods to transform the state it contains. Information about each of
 * the methods and their considerations can be found in the "STATIC METHODS",
 * "DYNAMIC METHODS", and "MEMBER METHODS" sections.
 ******************************************************************************/


/*
 * NOUN METHOD REFERENCES
 * The method functions must be forward-referenced so that they are visible
 * in the scope of the new_noun() function. Their implementation occurs
 * further down in the file.
 */
void method_noun_step(void *self, int dir);
void method_noun_setyx(void *self, int y, int x);
void method_noun_hit(void *self);
void method_noun_fall(void *self);
void method_noun_seek(void *self, void *target);
void method_noun_mobile(void *self, bool opt);
void method_noun_delete(void *self);
void method_noun_animate(void *self, void *animation);
void method_noun_doom(void *self);
void method_noun_update(void *self);
void method_noun_player(void *self, bool opt);

void member_method_noun_modify(void);
void member_method_noun_render(void);
void member_method_noun_update(void);
void member_method_noun_move(int dir);
void member_method_noun_fall(void);
void member_method_noun_hit(void);
void member_method_noun_seek(void *target);
void member_method_noun_setyx(int y, int x);
void member_method_noun_mobile(bool opt);
void member_method_noun_delete(void);
void member_method_noun_animate(void *animation);
void member_method_noun_take(int y, int x);
void member_method_noun_doom(void);
void member_method_noun_player(bool opt);


bool route_to_noun(void *self);


/**
 * CONSTRUCTOR
 * new_noun -- Create a new noun object and store it in the nountree
 * @name: the name of the noun
 * @model: the type enum of the noun
 * @obj: the class struct containing type-specific data
 */
struct noun_t *new_noun(const char *name, uint32_t model, void *obj)
{
        check_nountable();

        struct noun_t *new = calloc(1, sizeof(struct noun_t));

        /* Identity state */
        new->name  = cdup(name);
        new->id    = hash(name);
        new->model = model;
        new->obj   = obj;

        /* Member objects */
        new->sm    = new_sm(new->id, &route_to_noun);
        new->astar = new_astar();
        list_head_init(&new->inv);

        /* Boolean state */
        new->is_mobile   = false;
        new->hit_testing = true;

        /* Static methods */
        new->_step     = &method_noun_step;
        new->_hit      = &method_noun_hit;
        new->_fall     = &method_noun_fall;
        new->_seek     = &method_noun_seek;
        new->_setyx    = &method_noun_setyx;
        new->_mobile   = &method_noun_mobile;
        new->_animate  = &method_noun_animate;
        new->_del      = &method_noun_delete;
        new->_take     = &method_noun_take;
        new->_doom     = &method_noun_doom;
        new->_update   = &method_noun_update;
        new->_player   = &method_noun_player;

        /* Member methods */
        new->mobile   = &member_method_noun_mobile;
        new->step     = &member_method_noun_move;
        new->hit      = &member_method_noun_hit;
        new->fall     = &member_method_noun_fall;
        new->seek     = &member_method_noun_seek;
        new->setyx    = &member_method_noun_setyx;
        new->update   = &member_method_noun_update;
        new->animate  = &member_method_noun_animate;
        new->del      = &member_method_noun_delete;
        new->take     = &member_method_noun_take;
        new->doom     = &member_method_noun_doom;
        new->player   = &member_method_noun_player;

        /* Apply dynamic linkage */
        apply_noun_model(new);

        /* Store the noun */
        add_to_nountable(new);



        return new;
}





/* NOUN RETREIVAL AND QUERIES 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * NO FOCUSED
 * find_noun -- return a noun without setting 'focused'
 * @id: unsigned 32-bit unique id
 */
struct noun_t *find_noun(uint32_t id)
{
        return (struct noun_t *)htab_get(nountable, id);
}

/**
 * BY ID (KEY)
 * key_noun -- return a noun given its id number 
 * @id: unsigned 32-bit unique id
 */
struct noun_t *key_noun(uint32_t id)
{
        focused = htab_get(nountable, id);
        return (focused);
}

/**
 * BY VALUE (STRING)
 * get_noun -- return a noun given it's name string 
 * @name: name of the noun to be returned
 */
struct noun_t *get_noun(const char *name)
{
        uint32_t key = fasthash(name, strlen(name));

        return (htab_exists(nountable, key)) ? key_noun(key) : NULL;
}

/**
 * BY COORDINATES (Y,X)
 * get_noun_at -- given coordinates y, x, return any noun at that position
 * @y: y-coordinate
 * @x: x-coordinate
 */
struct noun_t *get_noun_at(struct map_t *map, int y, int x)
{
        return (key_noun(mx_val(map->mobs, y, x)));  
}

/**
 * NOUN MODEL 
 * noun_model -- return the model of the noun given an id number
 * @id: noun identifier 
 */
int noun_model(uint32_t id)
{
        struct noun_t *noun;
        noun = find_noun(id);
        return noun ? noun->model : NOMODEL;
}


struct noun_t *get_player(void)
{
        return PLAYER;
}



/* NOUN METHODS 
``````````````````````````````````````````````````````````````````````````````*/
/******************************************************************************
 * A noun object contains methods in two forms. One effects a direct method
 * call, providing a void *self argument to the method function, which
 * unambiguously references the caller (the noun object). We're calling those
 * "static methods" here. The other kind of method expects no arguments at all, 
 * but calls one of the static methods, providing a global object reference as
 * the void *self argument. These we will call "member methods".
 *
 * Each time a noun is retreived from the hash table, the object reference
 * (struct noun_t *focused) is set to the address of the retreived noun
 * object. If a method of the noun object is called immediately following 
 * retreival of that object, it can expect the address at 'focused' to be
 * the address of the caller.
 ******************************************************************************/

/**
 * PRIVATE METHOD HELPER
 * noun_mark_position -- update the position of the noun id on the map matrix */
static inline void noun_mark_position(struct noun_t *noun)
{
        mx_set(ACTIVE->mobs, pos_saved_y(noun->pos), pos_saved_x(noun->pos), 0);
        mx_set(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), noun->id);
}

/**
 * PRIVATE METHOD HELPER
 * hit_detected -- test whether a terrain collision is occuring */
bool hit_detected(struct noun_t *noun)
{
        return (map_hit(ACTIVE, noun) || mob_hit(ACTIVE, noun)) ? true : false;
}

/**
 * PRIVATE METHOD HELPER
 * noun_on_move -- called every time a noun's position is changed */
void noun_on_move(struct noun_t *noun)
{
        noun->_hit(noun);
        noun_mark_position(noun);

        move_panel(noun->pan, pos_y(noun->pos), pos_x(noun->pos));
        astar_set_start(noun->astar, pos_y(noun->pos), pos_x(noun->pos));
        take_bkgrnd(noun->win, PEEK(ACTIVE->W), FLEX);

        update_panels();

        if (DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)))
                door_trigger(noun, DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)));
}

/* STATIC METHODS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * _DEL METHOD
 * method_noun_delete -- delete a noun object
 * @self: the noun object
 */
void method_noun_delete(void *self)
{
        struct noun_t *noun = NOUN(self);

        del_panel(noun->pan);
        delwin(noun->win);

        del_sm(noun->sm);
        free(noun->sm);

        del_astar(noun->astar);
        free(noun->astar);

        mx_set(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), 0UL);
        noun->pos->del(noun->pos);

        /* Remove from the nountable */
        del_from_nountable(noun);

        /* Goodbye */
        /*free(noun);*/
}

/**
 * _UPDATE METHOD
 * method_noun_update -- modify and then render a noun object
 * @self: the noun object
 */
void method_noun_update(void *self)
{
        struct noun_t *noun = NOUN(self);

        noun->_modify(noun);
        noun->_render(noun);
}


/**
 * _MOBILE METHOD
 * method_noun_mobile -- set a noun to the active state
 * @self: the noun object
 */
void method_noun_mobile(void *self, bool opt)
{
        struct noun_t *noun = NOUN(self);
        noun->is_mobile = opt;
}


void method_noun_player(void *self, bool opt)
{
        struct noun_t *noun = NOUN(self);
        PLAYER = (opt) ? noun : NULL;
}



/**
 * _HIT METHOD
 * method_noun_hit -- perform the collision test
 * @self: the noun object
 */
void method_noun_hit(void *self)
{
        struct noun_t *noun = NOUN(self);

        if (noun->hit_testing && hit_detected(noun))
                noun->pos->restore(noun->pos);
}


/**
 * _SETYX METHOD
 * method_noun_setyx -- reposition the noun to coordinates y,x 
 * @self: the noun object
 * @y: y-coordinate to position noun at
 * @x: x-coordinate to position noun at
 */
void method_noun_setyx(void *self, int y, int x)
{
        struct noun_t *noun = NOUN(self);

        noun->pos->setyx(noun->pos, y, x);
        noun_on_move(noun);
}


/**
 * _STEP METHOD
 * method_noun_step -- move the noun in a given direction 
 * @self: the noun object
 * @dir: direction
 */
void method_noun_step(void *self, int dir)
{
        #define UR 231
        #define UL 225
        #define DR 214
        #define DL 208

        struct noun_t *noun = NOUN(self);

        switch (dir) {
        case 'u':       
                pos_u(noun->pos);
                break;
        case 'd':       
                pos_d(noun->pos);
                break;
        case 'l':       
                pos_l(noun->pos);
                break;
        case 'r':      
                pos_r(noun->pos);
                break;
        case UR:
                pos_u(noun->pos);
                pos_r(noun->pos);
                break;
        case UL:
                pos_u(noun->pos);
                pos_l(noun->pos);
                break;
        case DR:
                pos_d(noun->pos);
                pos_r(noun->pos);
                break;
        case DL:
                pos_d(noun->pos);
                pos_l(noun->pos);
                break;
        }
        noun_on_move(noun);
}


/**
 * _FALL METHOD
 * method_noun_fall -- shift a noun's position downward if gravity is enabled
 * @self: the noun object
 */
void method_noun_fall(void *self)
{
        struct noun_t *noun = NOUN(self);
        int y;
        int x;

        if (!gravity_enabled)
                return;

        y = pos_y(noun->pos);
        x = pos_x(noun->pos);

        INC(y, pos_ymax(noun->pos));

        if (TILE(ACTIVE, y, x) == CAVEFLOOR)
                noun->_step(noun, 'd');
}


/**
 * _SEEK METHOD
 * method_noun_seek -- run the A* function to seek a target noun
 * @self: the noun object
 * @target: the noun object to seek
 */
void method_noun_seek(void *self, void *target)
{
        struct noun_t *s = NOUN(self);
        struct noun_t *g = NOUN(target);

        if ((s->astar->current == NULL) 
        || !(same_cell(s->astar->current, g->astar->start))) 
        {
                if (!a_star(s->astar, g->astar->start))
                        return;
        }
        struct cell_t *tmp;
        tmp = cellpath_next(&s->astar->path);

        /* Sometimes there -is- no next */
        if (tmp) {
                if (tmp->x > s->astar->start->x)
                        sm_msg(s->sm, SM_SELF, SM_GoRight);
                if (tmp->x < s->astar->start->x)
                        sm_msg(s->sm, SM_SELF, SM_GoLeft);
                if (tmp->y > s->astar->start->y)
                        sm_msg(s->sm, SM_SELF, SM_GoDown);
                if (tmp->y < s->astar->start->y)
                        sm_msg(s->sm, SM_SELF, SM_GoUp);
        }
}


/**
 * _ANIMATE METHOD
 * method_noun_animate -- set the current animation
 * @self: the noun object
 * @animation: the new animation pointer
 */
void method_noun_animate(void *self, void *animation)
{
        struct noun_t *noun = NOUN(self);
        noun->animation = (struct ani_t *)animation;
        /*
         * Initialize animation length if it hasn't been set (lets us
         * declare inline struct ani_t's in model files)
         */
        if (noun->animation->len == 0)
                noun->animation->len = wcslen(noun->animation->wcs);

        wbkgrnd(noun->win, mkcch(noun->sprite, 0, noun->color));
}


/**
 * _DOOM METHOD
 * method_noun_doom -- doom the current noun to deletion
 * @self: the noun object
 */
void method_noun_doom(void *self)
{
        struct noun_t *noun = NOUN(self);
        struct noun_key *new; 

        new = calloc(1, sizeof(struct noun_key));

        new->key = noun->id;

        list_add(&doomring, &new->node);
}



/**
 * PRIVATE
 * route_to_noun -- the routing method for the state machine configuration */
bool route_to_noun(void *self)
{
        struct msg_t *msg = (struct msg_t *)self;
        struct noun_t *noun;
        bool status;

        noun = key_noun(msg_to(msg));

        if (noun != NULL) {
                assert((noun && msg) || !"Message router is boned!");
                status = sm_accept(noun->sm, msg);
                return (status) ? SM_ROUTE_OK : SM_NO_ROUTE; 
        }
        return SM_NO_ROUTE;
}



void noun_set_signal_delayed(struct noun_t *noun, uint32_t state, int dir)
{
        int y = pos_y(noun->pos);
        int x = pos_x(noun->pos);
        
        switch (dir) {
        case 'u':
                DEC(y, pos_ymin(noun->pos));
                break;
        case 'd':
                INC(y, pos_ymax(noun->pos));
                break;
        case 'l':
                DEC(x, pos_xmin(noun->pos));
                break;
        case 'r':
                INC(x, pos_xmax(noun->pos));
                break;
        default:
                break; /* Signal will be sent to the sender */
        }
        sm_msg(noun->sm, mx_val(ACTIVE->mobs, y, x), state);
}

void noun_set_signal(struct noun_t *noun, uint32_t state, int dir)
{
        noun_set_signal_delayed(noun, state, dir);
}


void emit_to_noun(struct noun_t *noun, int dir, uint32_t state)
{
        int y = pos_y(noun->pos);
        int x = pos_x(noun->pos);
        
        switch (dir) {
        case 'u':
                DEC(y, pos_ymin(noun->pos));
                break;
        case 'd':
                INC(y, pos_ymax(noun->pos));
                break;
        case 'l':
                DEC(x, pos_xmin(noun->pos));
                break;
        case 'r':
                INC(x, pos_xmax(noun->pos));
                break;
        default:
                break; /* Signal will be sent to the sender */
        }
        sm_msg(noun->sm, mx_val(ACTIVE->mobs, y, x), state);
}

/* MEMBER METHODS
``````````````````````````````````````````````````````````````````````````````*/
/******************************************************************************
 * PSEUDO MEMBER METHODS 
 *
 * These functions approximate the behavior of member functions in
 * class-based object oriented programming. These constructs are
 * not provided for in ANSI-C, but they are implemented here through
 * the use of a global accessor variable.
 *
 * The variable 'struct noun_t *focused' is set to the value last 
 * retreived from the nountree. Provided this access was accomplished
 * immediately prior to calling one of these member functions, they
 * may assume the caller to be the noun at the address pointed to by
 * 'focused'. 
 *
 * Using the nn() accessor, which expands to get_noun(), some typical 
 * usages might look like:
 *
 *      nn("Ophelia")->move('d');
 *      nn("Ophelia")->update();
 *      
 ******************************************************************************/ 
/**
 * DESTRUCTOR METHOD
 * member_method_noun_delete -- calls the delete method */
void member_method_noun_delete(void)
{
        focused->_del(focused);
}

/**
 * MODIFY METHOD
 * member_method_noun_modify -- calls the modify method */
void member_method_noun_modify(void)
{
        focused->_modify(focused);
}

/**
 * RENDER METHOD
 * member_method_noun_render -- calls the render method */
void member_method_noun_render(void)
{
        focused->_render(focused);
}

/**
 * UPDATE METHOD
 * member_method_noun_update -- calls the modify and render methods together */
void member_method_noun_update(void)
{
        focused->_modify(focused);
        focused->_render(focused);
}

/**
 * MOBILE METHOD
 * member_method_noun_activate -- calls the activate method */
void member_method_noun_mobile(bool opt)
{
        focused->_mobile(focused, opt);
}

/**
 * MOVE METHOD
 * member_method_noun_move -- call the noun's step method
 * @dir: direction of step */
void member_method_noun_move(int dir)
{
        focused->_step(focused, dir);
}

/**
 * SEEK METHOD
 * member_method_noun_sekk -- seek the target noun with A* pathfinding */
void member_method_noun_seek(void *target)
{
        focused->_seek(focused, target);
}

/**
 * FALL METHOD
 * member_method_noun_fall -- call the noun's fall method */
void member_method_noun_fall(void)
{
        focused->_fall(focused);
}

/**
 * HIT METHOD
 * member_method_noun_hit -- call the noun's hit method */
void member_method_noun_hit(void)
{
        focused->_hit(focused);
}

/**
 * SETYX METHOD
 * member_method_noun_setyx -- call the noun's setyx method */
void member_method_noun_setyx(int y, int x)
{
        focused->_setyx(focused, y, x);
}

/**
 * ANIMATE METHOD
 * member_method_noun_animate -- set the current animation
 * @animation: the new animation pointer
 */
void member_method_noun_animate(void *animation)
{
        focused->_animate(focused, animation);
}


/**
 * TAKE METHOD
 * member_method_noun_take -- add an item to your inventory 
 * @eq_id : the hash key of the equipment item
 */
void member_method_noun_take(int y, int x)
{
        focused->_take(focused, y, x);
}


void member_method_noun_player(bool opt)
{
        PLAYER = (opt) ? focused : NULL;
}


/**
 * DOOM METHOD
 * member_method_noun_doom -- doom a noun to deletion
 */
void member_method_noun_doom(void)
{
        focused->_doom(focused);
}



/* ID TRACKER
``````````````````````````````````````````````````````````````````````````````*/
bool     id_ready;
uint32_t active_id[2];


/**
 * install_id -- make an id part of the active pair
 * @id: id number
 * @opt: SUBJECT or OBJECT
 */
void install_id(uint32_t id, int opt)
{
        assert(opt==OBJECT || opt==SUBJECT || "Noun request is invalid");
        active_id[opt] = id;
}


/**
 * request_id -- request one of the current active id pair
 * @opt: SUBJECT or OBJECT
 */
uint32_t request_id(int opt)
{
        assert(opt==OBJECT || opt==SUBJECT || "Noun request is invalid");

        struct noun_key *tmp;

        if (!id_ready) {
                tmp = list_top(&keyring, struct noun_key, node);
                active_id[0] = active_id[1] = tmp->key;
                id_ready = true;
        }
        return (active_id[opt]);
}


