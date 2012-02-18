#include "../gfx/zbox.h"

typedef struct the_whole_world {
        uint32_t h;
        uint32_t w;
        uint32_t n;
        double **pmap;
        WINDOW *L[16];
        WINDOW *W;
        ZBOX *Z;
} MAP;

extern MAP *GLOBE;


void draw_layers(MAP *map);
MAP *worldgen(uint32_t rows, uint32_t cols);
void roll(MAP *map, int dir);
