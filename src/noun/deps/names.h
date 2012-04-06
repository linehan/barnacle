#pragma once
#ifndef __NAMES_MODEL
#define __NAMES_MODEL
#include <string.h>
#include "../noun_model.h"

static inline char *fullname(uint32_t key)
{
        focus(key);
        return (focused->name);
}

#endif
