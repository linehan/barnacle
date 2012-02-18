//void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0);
//void shift_highlights(PLATE *pl);
//void highlights_init(PLATE *pl);
//void gen_terrain(PLATE *pl, char type, int h, int w, int y0, int x0);

//void connected_components(PLATE *pl, int type);
//void simpledraw(PLATE *pl);

//void testworld(int size_factor);
#include "../gfx/zbox.h"

typedef struct the_whole_world {
        unsigned int h;
        unsigned int w;
        unsigned int n;
        double **pmap;
        WINDOW *L[16];
        WINDOW *W;
        ZBOX *Z;
} MAP;

void draw_layers(MAP *map);
MAP *worldgen(int rows, int cols);
