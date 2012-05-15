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



void method_noun_step(void *self, int dir);
void method_noun_setyx(void *self, int y, int x);
void method_noun_hit(void *self);
void method_noun_fall(void *self);
void method_noun_seek(void *self, void *target);


#define NOUN(ptr) (struct noun_t *)(ptr)
#define MAXNOUN 1000


struct rb_tree *nountree; /* Holds all registered nouns */
int numnoun;              /* Counts all registered nouns */




/* Helper functions
``````````````````````````````````````````````````````````````````````````````*/
/**
 * PRIVATE 
 * check_nountree -- ensure the tree is initialized and allocated */ 
inline void check_nountree(void)
{
        if (!nountree) { 
                nountree = rb_new(1);
                numnoun  = 0;
        }
        assert(nountree || "Could not allocate noun tree");
}

/**
 * PRIVATE
 * add_to_nountree -- insert a noun into the nountree and refcount */
inline void add_to_nountree(struct noun_t *noun)
{
        rb_store(nountree, noun->id, noun); 
        keyring[numnoun++] = noun->id;

        assert(numnoun < MAXNOUN || "Noun limit reached");
}

/**
 * PRIVATE
 * emit -- the verb sending method for the state machine configuration
 */
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
        check_nountree();

        struct noun_t *new = malloc(sizeof(struct noun_t));

        new->name  = mydup(name);
        new->id    = fasthash(name, strlen(name));
        new->model = model;
        new->obj   = obj;

        new->inv   = new_inventory(new);
        new->astar = new_astar();

        new->sm    = new_sm(new->id, &emit_verb);
        sm_active(new->sm, true);

        new->is_active   = false;
        new->hit_testing = true;

        new->step  = &method_noun_step;
        new->hit   = &method_noun_hit;
        new->fall  = &method_noun_fall;
        new->seek  = &method_noun_seek;
        new->setyx = &method_noun_setyx;

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
 * noun_set_animation -- set the currently active animation 
 * @noun: pointer to a struct noun_t
 * @ani: pointer to a struct ani_t 
 */
void set_animation(struct noun_t *noun, struct ani_t *ani)
{
        noun->animation = ani;
}


/**
 * noun_active -- toggle a noun's visibility on-screen
 * @noun: Pointer to struct noun_t
 * @opt: Whether or not the noun should be visible
 */
void noun_active(struct noun_t *noun, bool opt)
{
        noun->is_active = opt;
        (noun->is_active) ? show_panel(noun->pan) : hide_panel(noun->pan);
}





/* Queries and retreival 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * key_noun -- return a noun given its id number 
 * @id: unsigned 32-bit unique id
 */
struct noun_t *key_noun(uint32_t id)
{
        focused = rb_extra(nountree, id);
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
 * hit_detected -- test whether a terrain collision is occuring 
 * @noun: pointer to struct noun_t 
 */
inline bool hit_detected(struct noun_t *noun)
{
        return (map_hit(ACTIVE, noun->pos));
}


/**
 * PRIVATE
 * noun_on_move -- called every time a noun's position is changed
 * @noun: pointer to a struct noun_t
 */
inline void noun_on_move(struct noun_t *noun)
{
        noun_mark_position(noun);
        noun->hit(noun);

        move_panel(noun->pan, pos_y(noun->pos), pos_x(noun->pos));
        astar_set_start(noun->astar, pos_y(noun->pos), pos_x(noun->pos));
        take_bkgrnd(panel_window(noun->pan), PEEK(ACTIVE->W));

        scr_refresh();

        if (DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)))
                door_trigger(noun, DOOR(ACTIVE, pos_y(noun->pos), pos_x(noun->pos)));
}





/* METHODS
``````````````````````````````````````````````````````````````````````````````*/
/**
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
                noun->step(noun, 'd');
}


/**
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

