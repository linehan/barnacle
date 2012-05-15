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

#define NOUN(ptr) (struct noun_t *)(ptr)
#define MAXNOUN 1000



struct hashtable_t *nountable;
int numnoun;



/* Helper functions
``````````````````````````````````````````````````````````````````````````````*/
/**
 * PRIVATE 
 * check_nountable -- ensure the hashtable is initialized and allocated */ 
inline void check_nountable(void)
{
        if (!nountable) { 
                nountable = new_hashtable(0);
                numnoun  = 0;
        }
        assert(nountable || "Could not allocate noun table");
}

/**
 * PRIVATE
 * add_to_nountable -- insert a noun into the noun hash table and refcount */
inline void add_to_nountable(struct noun_t *noun)
{
        hashtable_add(nountable, noun->id, noun); 
        keyring[numnoun++] = noun->id;

        assert(numnoun < MAXNOUN || "Noun limit reached");
}

/**
 * PRIVATE
 * emit_verb -- the verb sending method for the state machine configuration */
bool emit_verb(void *self)
{
        struct verb_t *verb = (struct verb_t *)self;
        struct noun_t *noun;

        if (noun = key_noun(verb->to), noun) {
                sm_set(noun->sm, verb->name, verb->value);
                return true;
        }
        return false;
}



/* Method forward references
``````````````````````````````````````````````````````````````````````````````*/
void method_noun_step(void *self, int dir);
void method_noun_setyx(void *self, int y, int x);
void method_noun_hit(void *self);
void method_noun_fall(void *self);
void method_noun_seek(void *self, void *target);
void method_noun_mobile(void *self, bool opt);

void member_method_noun_modify(void);
void member_method_noun_render(void);
void member_method_noun_update(void);
void member_method_noun_move(int dir);
void member_method_noun_fall(void);
void member_method_noun_hit(void);
void member_method_noun_seek(void *target);
void member_method_noun_setyx(int y, int x);
void member_method_noun_mobile(bool opt);

/* Noun destructor
``````````````````````````````````````````````````````````````````````````````*/
/*void destroy_noun(void *self)*/
/*{*/
        /*struct noun_t *noun = NOUN(self);*/

        /* Remove from the nountree */

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
        check_nountable();

        struct noun_t *new = malloc(sizeof(struct noun_t));

        new->name  = mydup(name);
        new->id    = fasthash(name, strlen(name));
        new->model = model;
        new->obj   = obj;

        new->inv   = new_inventory(new);
        new->astar = new_astar();

        new->sm    = new_sm(new->id, &emit_verb);
        sm_active(new->sm, true);

        new->is_mobile   = false;
        new->hit_testing = true;

        /* Assign static methods */
        new->_step     = &method_noun_step;
        new->_hit      = &method_noun_hit;
        new->_fall     = &method_noun_fall;
        new->_seek     = &method_noun_seek;
        new->_setyx    = &method_noun_setyx;
        new->_mobile   = &method_noun_mobile;

        /* Assign member methods */
        new->mobile   = &member_method_noun_mobile;
        new->step     = &member_method_noun_move;
        new->hit      = &member_method_noun_hit;
        new->fall     = &member_method_noun_fall;
        new->seek     = &member_method_noun_seek;
        new->setyx    = &member_method_noun_setyx;
        new->update   = &member_method_noun_update;

        apply_noun_model(new);
        add_to_nountable(new);

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
        noun->_render = func;
}


/**
 * noun_set_modify -- set the modify method of the noun
 * @noun: pointer to a struct noun_t
 * @func: pointer to a function returning int and accepting void * and int
 */
void noun_set_modify(struct noun_t *noun, MODIFY_METHOD func)
{
        noun->_modify = func;
}


/**
 * noun_set_animation -- set the currently active animation 
 * @noun: pointer to a struct noun_t
 * @ani: pointer to a struct ani_t 
 */
void set_animation(struct noun_t *noun, struct ani_t *ani)
{
        noun->animation = ani;
}








/* Queries and retreival 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * key_noun -- return a noun given its id number 
 * @id: unsigned 32-bit unique id
 */
struct noun_t *key_noun(uint32_t id)
{
        focused = hashtable_get(nountable, id);
        return (focused);
}


/**
 * get_noun -- return a noun given it's name string 
 * @name: name of the noun to be returned
 */
struct noun_t *get_noun(const char *name)
{
        return (key_noun(fasthash(name, strlen(name))));
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





/* METHOD HELPERS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * PRIVATE
 * noun_mark_position -- update the position of the noun id on the map matrix */
inline void noun_mark_position(struct noun_t *noun)
{
        mx_set(ACTIVE->mobs, pos_saved_y(noun->pos), pos_saved_x(noun->pos), 0);
        mx_set(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), noun->id);
}


/**
 * PRIVATE
 * hit_detected -- test whether a terrain collision is occuring */
inline bool hit_detected(struct noun_t *noun)
{
        return (map_hit(ACTIVE, noun->pos));
}


/**
 * PRIVATE
 * noun_on_move -- called every time a noun's position is changed */
inline void noun_on_move(struct noun_t *noun)
{
        noun_mark_position(noun);
        noun->_hit(noun);

        move_panel(noun->pan, pos_y(noun->pos), pos_x(noun->pos));
        astar_set_start(noun->astar, pos_y(noun->pos), pos_x(noun->pos));
        take_bkgrnd(panel_window(noun->pan), PEEK(ACTIVE->W));

        scr_refresh();

        if (DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)))
                door_trigger(noun, DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)));
}





/* STATIC METHODS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * _MOBILE METHOD
 * method_noun_mobile -- set a noun to the active state
 * @self: the noun object
 */
void method_noun_mobile(void *self, bool opt)
{
        struct noun_t *noun = NOUN(self);

        noun->is_mobile = opt;
                
        if (noun->is_mobile)
                show_panel(noun->pan);
        else    hide_panel(noun->pan);
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

        if (tmp->x > s->astar->start->x)
                sm_set(s->sm, SM_GoRight, 0);
        if (tmp->x < s->astar->start->x)
                sm_set(s->sm, SM_GoLeft, 0);
        if (tmp->y > s->astar->start->y)
                sm_set(s->sm, SM_GoDown, 0);
        if (tmp->y < s->astar->start->y)
                sm_set(s->sm, SM_GoUp, 0);
}





void noun_set_signal(struct noun_t *noun, enum sm_state verb, int dir)
{
        int y = pos_y(noun->pos);
        int x = pos_x(noun->pos);
        
        switch (dir) {
        case 'u':
                DEC(y, pos_xmin(noun->pos));
                break;
        case 'd':
                INC(y, pos_ymax(noun->pos));
                break;
        case 'l':
                DEC(x, pos_ymin(noun->pos));
                break;
        case 'r':
                INC(x, pos_xmax(noun->pos));
                break;
        }
        sm_emit(noun->sm, mx_val(ACTIVE->mobs, y, x), 0, verb);
}


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

        if (!id_ready) {
                active_id[0] = active_id[1] = keyring[0];
                id_ready = true;
        }
        return (active_id[opt]);
}

