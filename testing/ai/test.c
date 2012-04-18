#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>

#include "bh.h"
#include "pathfind.h"


void ncurses_start(void)
{
        initscr();
        cbreak();	       /* Do not wait for newline (line buffering) */
        noecho();              /* Do not echo input to screen */
        curs_set(0);           /* Do not display cursor */
}


int main(void) 
{

        /*heap_test();*/
        /*return 0;*/

        ncurses_start();
        
        #define SIZE 30 
        struct matrix_t *map;
        int i;
        int j;

        map = new_matrix(SIZE, SIZE);

        print_matrix(map); 
        for (i=0; i<SIZE; i++) {
                for (j=0; j<SIZE; j++) {
                        mx_put(map, i, j, 0);
                }
        }

        for (i=0; i<SIZE; i++) {
                mx_put(map, 5, i, 3);
                mx_put(map, 18, i, 3);
        }
        mx_put(map, 5, 3, 0);
        mx_put(map, 5, 4, 0);
        mx_put(map, 18, 22, 0);
        mx_put(map, 18, 23, 0);
        print_matrix(map);

        /*print_map(map);*/

        astar(map, SIZE, SIZE, 0, 0, 25, 25);

        getch();

        endwin(); /* end curses mode */
        return 0;
}


