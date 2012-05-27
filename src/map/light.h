#include "../noun/noun.h"

struct light_t {
        WINDOW *win;
        PANEL *pan;
        bool lit;
        int num_colors;
        int y;
        int x;
        int yul;
        int xul;
        int h;
        int w;
        short base;
        short *pair;
        short *bg;
        short *fg;
};

void glow_light(struct light_t *light, struct noun_t *noun, bool move);
struct light_t *new_light(int h, int w, int y, int x, short base);
