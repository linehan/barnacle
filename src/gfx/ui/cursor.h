#ifndef __CURSOR_H
#define __CURSOR_H
#include "../../lib/pos.h"
#include "../../com/barnacle.h"

enum keycluster { HJKL, WASD, ARROWS, NUMPAD };


struct cursor_t {
        WINDOW *win;
        PANEL  *pan;
        struct pos_t *pos;
        enum keycluster keys;
        int step;
        void (*Hide)(void *self);
        void (*Show)(void *self);
        void (*Stamp)(void *self, WINDOW *win);
        void (*Move)(void *self, int input);
};


struct cursor_t *new_cursor(int h_cur, int w_cur, int y_cur, int x_cur,
                            int h_scr, int w_scr, int y_scr, int x_scr,
                            enum keycluster keys);

#endif
