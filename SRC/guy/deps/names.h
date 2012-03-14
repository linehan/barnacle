#ifndef __MODEL_NAMES
#define __MODEL_NAMES
#include <string.h>
#include "../guy_model.h"

static inline char *fname(uint32_t key)
{
        focus(key);
        return (focused->fname);
}
static inline char *mname(uint32_t key)
{
        focus(key);
        return (focused->mname);
}
static inline char *lname(uint32_t key)
{
        focus(key);
        return (focused->lname);
}

static char *flname(uint32_t key)
{
        char *f, *l;

        f = fname(key);
        l = lname(key);

        char *fl = malloc(strlen(f) + strlen(l) + 2);

        sprintf(fl, "%s %s", f, l);

        return (fl);
}

static char *fullname(uint32_t key)
{
        char *a, *b, *c;

        a = fname(key);
        b = mname(key);
        c = lname(key);

        char *abc = malloc(strlen(a) + strlen(b) + strlen(c) + 3);

        sprintf(abc, "%s %s %s", a, b, c);

        return (abc);
}

#endif
