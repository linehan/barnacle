#include <stdint.h>
#include "../gen/dice.h"
/* Implement the inside-out version of the Fisher-Yates shuffle algorithm,
 * which generates a random permutation of a finite set. Here, we use the
 * algorithm to shuffle the permutation table. */
inline void fy_shuffle(uint8_t *source, uint8_t *shuffle, int n)
{
        int i, j;
        shuffle[0] = source[0];
        for (i=1; i<(n-1); i++) {
             j = roll_fair(i);
             shuffle[i] = shuffle[j];
             shuffle[j] = source[i];
        }
}
