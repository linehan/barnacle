#include "../lib/textutils.h"

#include "gloss.h"


////////////////////////////////////////////////////////////////////////////////
void destroy_gloss(void *self)
{
       struct gloss_t *gloss = (struct gloss_t *)self; 

       free(gloss->wcs);
       free(gloss);
}

struct gloss_t *new_gloss(WINDOW *win, const char *str, wchar_t *wcs, short color, short highlight)
{
        struct gloss_t *new = malloc(sizeof(struct gloss_t));

        new->win = win;

        if (str!=NULL) new->wcs = mbdup(str);
        else           new->wcs = wcdup(wcs);

        new->len = wcslen(new->wcs);
        new->co  = color;
        new->hi  = highlight;

        new->put[REVEAL]   = reveal;
        new->put[UNREVEAL] = unreveal;
        new->put[SHINE]    = shine;
        new->put[PUSH_R]   = pushr;
        new->put[PUSH_L]   = pushl;

        new->die           = &destroy_gloss;

        return (new);
}
