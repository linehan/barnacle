<<<<<<< .merge_file_VJqGnz

/* A mobile data type containing some object */
struct mob_t {
        struct ufo_t *ufo;
        PANEL *pan;
        sem_t sem;
};

struct mob_t *new_mob(struct map_t *map, int h, int w, int y0, int x0);
void move_mob(struct mob_t *mob, int dir);
=======
/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        DIMS   dim;
        //PLATE *pl;
        sem_t *sem;
        void  *obj;
} MOB;

MOB *new_mob(void *ptr, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);
>>>>>>> .merge_file_hNJ1hz
