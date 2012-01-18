#include <pthread.h>
#include <semaphore.h>
/* ========================================================================== */
enum ids { __bg__ = 0,
           __fg__ = 1,
           __an__ = 2
};
/* A structure that bundles all the window dimensions */
   typedef struct dimension_t {
     int h;  /* window height */
     int w;  /* window width  */
     int y0; /* window initial y */
     int x0; /* window initial x */
     int dy; /* window current y */
     int dx; /* window current x */
     int n;  /* number of windows */
   } DIMS;

typedef struct environment_t {
        struct list_head wad;
        sem_t *sem;
} ENV;

/* A mobile data type containing some object */
   typedef struct mob_t {
     PANEL *pan;
     DIMS   dim;
     sem_t *sem;
     ENV *env;
     void  *obj;
   } MOB;



/* A node in a linked list ("wad") of windows */
   typedef struct win_wad {
      WINDOW *window;
      struct list_node winnode; 
   } WINNODE;

/* A node in a linked list ("wad") of graphics */
   typedef struct gfx_wad {
      struct list_head winwad; /* a winwad */
      int id;
      PANEL *pan;
      DIMS  *dim;
      struct list_node gfxnode;
   } GFXNODE;

enum pan_options { 
        __pan__ = 0,
        __win__ = 1,
        __dim__ = 2,
        __sem__ = 3,
        __obj__ = 4,
        __h__   = 5, 
        __w__   = 6, 
        __y0__  = 7,
        __x0__  = 8, 
        __dy__  = 9, 
        __dx__  = 10, 
        __n__   = 11
};
/* ========================================================================== */

void gfx_init(void);
void add_gfx(GFXNODE *node, struct list_head *wad);
GFXNODE *new_gfx(int id, int h, int w, int y0, int x0, int n);
void master_refresh(void);
MOB *new_mob(void *ptr, ENV *env, int h, int w, int y0, int x0);

void move_mob(MOB *mob, int dir);
ENV *new_env(void);
int hit_test(ENV *env, int y, int x);
void step_all_env(ENV *env);
