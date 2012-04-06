#include <stdlib.h>
#include "stoc.h"

/*
 * Start the stochastic features
 */
void init_stochastics(void)
{
        mt_check();
        init_simplex();   /* Initialize the simplex noise generator */
}


