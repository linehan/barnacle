#pragma once
#ifndef __VITALS_MODEL
#define __VITALS_MODEL
#include <stdint.h>
#include <stdbool.h>
#include "../noun.h"

static const int VIT_MAXLEN=32;
static const int VIT_GRAIN=24; // Each block represents this many stat points.

static const int VIT_MAX_BLOCK[]={5, 8, 5, 3};
static const int VIT_MIN_BLOCK[]={1, 1, 1, 1};

//////////////////////////////////////////////
//            Health       Lucidity         //
//                \        /                //            
enum vitals_enum {HP, SP, LP, EP};          //
//                    |        \            //
//                  Stamina    Emotion      //
//////////////////////////////////////////////


static inline uint32_t get_vital(int id, int b)
{
        key_noun(id);
        return (get_byte(focused->vitals, b));
}

static inline void set_vital(int id, int b, int s)
{
        key_noun(id);
        set_byte(&focused->vitals, b, s);
}

static inline void unpack_vitals(int id, int *dest)
{
        key_noun(id);
        unpack_bytes(focused->vitals, dest, NBYTES);
}

static inline void inc_vital(int id, int b)
{
        key_noun(id);
        set_byte(&focused->vitals, b, (get_byte(focused->vitals, b)+1));
}

static inline void dec_vital(int id, int b)
{
        key_noun(id);
        set_byte(&focused->vitals, b, (get_byte(focused->vitals, b)-1));
}

static inline int vit_blocklen(int id)
{
        int accumulate=0;
        int i;

        for (i=0; i<4; i++) {
                accumulate += get_vital(id, i);
        }

        return accumulate;
}
#endif

