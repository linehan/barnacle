#include "../geo/path_mvc.h"


/* A mobile data type containing some object */
struct mob_t {
        struct ufo_t *ufo;
        struct path_t path;
        PANEL *pan;
        sem_t sem;
};

struct mob_t *new_mob(struct map_t *map, int h, int w, int y0, int x0);
void move_mob(struct mob_t *mob, int dir);
