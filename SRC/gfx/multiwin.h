#pragma once
#ifndef __MULTIWIN_H
#define __MULTIWIN_H

#include "gfx.h"

struct multiwin_t {
        unsigned int n;
        unsigned int i;
        WINDOW **win;
        WINDOW *peek;
        void (*next)(void *self);
        void (*prev)(void *self);
};

struct multiwin_t *new_multiwin(int h, int w, int y0, int x0, int n);

#define PEEK(multiwin) (multiwin)->peek
#define NEXT(multiwin) multiwin->next((multiwin))
#define PREV(multiwin) multiwin->prev((multiwin))

#endif
