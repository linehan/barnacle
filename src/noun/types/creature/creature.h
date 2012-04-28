#include <stdlib.h>
#include "../../../gfx/gfx.h"

#define NUMDIRECTIONS 4
enum signal_direction {N, E, S, W};

struct person_t {
        cchar_t cch;
};

void new_creature(const char *name, int job, const wchar_t *wch, short pair);
int modify_human(void *obj, int verb, int value);
