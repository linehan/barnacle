#pragma once
#ifndef __GLOSS_MODEL
#define __GLOSS_MODEL

#define NSTYLES 5
enum glosses { 
        REVEAL, 
        UNREVEAL, 
        SHINE, 
        PUSH_R, 
        PUSH_L 
};


////////////////////////////////////////////////////////////////////////////////
struct gloss_t {
        WINDOW *win;
        enum glosses;
        wchar_t *wcs;
        int len;
        short co;
        short hi;
        void (*put[NSTYLES])(void *self, int loop);
        void (*die)(void *self);
};


struct gloss_t *
new_gloss(WINDOW *win, const char *str, wchar_t *wcs, short color, short highlight);

#endif
