/* ========================================================================== */
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

/* A mobile data type containing some object */
   typedef struct mob_t {
     PANEL *pan;
     DIMS   dim;
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
      PANEL *pan;
      DIMS  *dim;
      struct list_node gfxnode;
   } GFXNODE;
/* ========================================================================== */

void add_gfx(GFXNODE *node, struct list_head *wad);
GFXNODE *new_gfx(int h, int w, int y0, int x0, int n);
MOB *new_mob(void *ptr, int h, int w, int y0, int x0);
