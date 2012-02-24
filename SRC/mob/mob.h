/* A mobile data type containing some object */
struct mob_t {
        PANEL *pan;
        struct ufo_t *ufo;
        struct sw_t *sw;
        sem_t *sem;
};

struct mob_t *new_mob(struct map_t *map, int h, int w, int y0, int x0);
void move_mob(struct mob_t *mob, int dir);
