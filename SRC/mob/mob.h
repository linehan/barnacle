/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        sem_t *sem;
        struct ufo_t *ufo;
        void  *obj;
} MOB;

MOB *new_mob(struct map_t *map, void *ptr, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);
