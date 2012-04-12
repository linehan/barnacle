#pragma once
#ifndef __NAMES_MODEL
#define __NAMES_MODEL
#include <string.h>
#include "../noun_model.h"

static inline char *fullname(int id)
{
        key_noun(id);
        return (focused->name);
}

#endif
