#include <stdlib.h>
#include "../../../gfx/gfx.h"


struct person_t {
        cchar_t cch;
};

void new_creature(const char *name, int job, const wchar_t *wch, short pair);
int modify_creature(void *obj, int input);
