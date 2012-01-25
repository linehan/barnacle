#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static signed int p[256];

void fy_shuffle(int *source, int *shuffled, int n)
{
        int i, j, struck;

        for (i=(n-1); i<1; i--) {
             int j = roll_fair(255);
             struck = source[j];
             source[j] = source[i];
             source[i] = struck;
        }
}
