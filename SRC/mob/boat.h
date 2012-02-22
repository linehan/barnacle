int init_boat(void);

void nominate_boat(MOB *mob);

MOB *new_boat(struct map_t *map);

void sync_boat(void);

int get_boat(MOB *mob, int a, int b);
int set_boat(MOB *mob, int a, int b, int val);

MOB *_BOAT;
void *sail_boat(void *);

void order_boat(int, int);

int get_pointofsail(void);



void deck_init(void);

void draw_deck(void);

void tog_deck(void);

enum boat_options {
        __now__ = 0, 
        __req__ = 1, 
        __buf__ = 2, 

        __hdg__ = 3, 
        __rig__ = 4, 
        __anc__ = 5
};
