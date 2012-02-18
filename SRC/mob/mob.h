/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        DIMS   dim;
        PLATE *pl;
        sem_t *sem;
        void  *obj;
} MOB;

MOB *new_mob(void *ptr, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);
