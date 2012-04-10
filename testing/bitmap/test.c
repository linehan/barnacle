#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ncurses.h>

#include "bitmap.h"

int main(void) 
{
        int rows = 20;
        int cols = 20;
        int i; /* row increment */
        int j; /* column increment */

        BITMAP *myb = new_bitmap(rows, cols);

        BITMAPSET(myb, 7, 11);
        BITMAPSET(myb, 18, 5);

        bitmap_for_each_row(myb, i) {
                printf("\n%02u  ", i);
                bitmap_for_each_col(myb, j) {
                        printf("%u ", BITMAPTEST(myb, i, j));
                }
        }

        printf("\n\n");
        myb->die(myb);

        return 0;
}

