#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ncurses.h>

#include "bitmap.h"

int main(void) 
{
        int rows = 8;
        int cols = 32;
        int i; /* row increment */
        int j; /* column increment */

        struct matrix_t *myb = new_bitmap(rows, cols);
        struct matrix_t *mym = new_matrix(rows, cols);

        BITMAPSET(myb, 7, 11);
        BITMAPSET(myb, 1, 5);

        bitmap_for_each_row(myb, i) {
                printf("\n%02u  ", i);
                bitmap_for_each_col(myb, j) {
                        printf(FHEX8" ", BITMAPVAL(myb, i, j));
                }
        }
        printf("\nbitmap size: %zu bytes", BM_size(myb));

        printf("\n\n");

        mx_put(mym, 5, 5, 8);
        mx_put(mym, 4, 5, 2);
        mx_put(mym, 6, 5, 5);
        mx_put(mym, 5, 4, 1);
        mx_put(mym, 5, 6, 7);
        mx_put(mym, 7, 5, 9);

        for (i=0; i<mym->rows; i++) {
                printf("\n%02u  ", i);
                for (j=0; j<mym->cols; j++) {
                        printf("%u ", mx_val(mym, i, j));
                }
        }

        printf("\n\nitem at    %d,%d: %u \n", 5, 5, mx_val(mym, 5, 5));
        printf("item N of  %d,%d: %u \n", 5, 5, *mx_n(mym, 5, 5));
        printf("item S of  %d,%d: %u \n", 5, 5, *mx_s(mym, 5, 5));
        printf("item E of  %d,%d: %u \n", 5, 5, *mx_e(mym, 5, 5));
        printf("item W of  %d,%d: %u \n", 5, 5, *mx_w(mym, 5, 5));
        printf("item NW of  %d,%d: %u \n", 5, 5, *mx_nw(mym, 5, 5));
        printf("item NE of  %d,%d: %u \n", 5, 5, *mx_ne(mym, 5, 5));
        printf("item SW of  %d,%d: %u \n", 5, 5, *mx_sw(mym, 5, 5));
        printf("item SE of  %d,%d: %u \n", 6, 5, *mx_se(mym, 7, 5));

        printf("\n\n");
        myb->die(myb);
        mym->die(mym);

        return 0;
}

