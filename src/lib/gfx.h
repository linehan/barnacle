typedef struct dimension_t {
  int h;
  int w;
  int y0;
  int x0;
  int dy;
  int dx;
  int n; /* number of windows */
} DIMS;

typedef struct win_wad {
  WINDOW *window;
  struct list_node winnode; 
} WINWAD;

typedef struct gfx_wad {
  struct list_head winwad;
  PANEL *pan;
  DIMS  *dim;
  struct list_node gfxnode;
} GFX;

void add_gfx(GFX *node, struct list_head *wad);
GFX *new_gfx(int h, int w, int y0, int x0, int n);
struct list_head *new_wad(void);
GFX *new_gfx(int h, int w, int y0, int x0, int n);

