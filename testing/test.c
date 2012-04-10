#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ncurses.h>

#include "matrix.h"

int main(void) 
{
        int rows;        
        int cols;
        int i; // row increment
        int j; // column increment

        rows = 20;
        cols = 20;

        /* traditional nested loops */
        
        struct matrix_t *mymatrix = new_matrix(rows, cols);

        matrix_for_each_row(mymatrix) {
                printf("\n");
                matrix_for_each_col(mymatrix) {
                        printf("%g ", matrix_current_element(mymatrix));
                }
        }

        printf("\n\n");


        /* for each loop */

        struct matrix_t *myothermatrix = new_matrix(1, 7);

        matrix_for_each(myothermatrix) {
                printf("%g ", matrix_current_element(myothermatrix));
        }}

        printf("\n\n");

        mymatrix->die(mymatrix);
        myothermatrix->die(myothermatrix);

        return 0;
}
