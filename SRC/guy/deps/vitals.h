#ifndef MODEL_VITALS
#define MODEL_VITALS
#include <stdint.h>
#include <stdbool.h>
#include "../guy_model.h"

static const int VIT_MAXLEN=30;
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


static inline uint32_t get_vital(uint32_t key, int b)
{
        focus(key);
        return (get_byte(focused->vitals, b));
}

static inline void set_vital(uint32_t key, int b, int s)
{
        focus(key);
        set_byte(&focused->vitals, b, s);
}

static inline void unpack_vitals(uint32_t key, int *dest)
{
        focus(key);
        unpack_bytes(focused->vitals, dest, NBYTES);
}

static inline int vit_blocklen(uint32_t key)
{
        int accumulate=0;
        int i;

        for (i=0; i<4; i++) {
                accumulate += get_vital(key, i);
        }

        return accumulate;
}
#endif
