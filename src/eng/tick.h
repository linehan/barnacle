#pragma once
#ifndef __ENG_TICK_H
#define __ENG_TICK_H


#include "../com/arawak.h"
#include <stdlib.h>


uint32_t TICKER;


static inline void tick(void)
{
        TICKER++;
}

static inline uint32_t get_tick(void)
{
        return (TICKER);
}

#endif

