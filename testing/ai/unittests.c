#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <string.h>

#include "bh.h"
#include "pathfind.h"
#include "matrix.h"
#include "pathfind.h"


void print_matrix(struct matrix_t *map)
{
        int i;
        int j;
        for (i=0; i<map->rows; i++) {
                printf("\n");
                for (j=0; j<map->cols; j++) {
                        printf("%u ", mx_val(map, i, j));
                }
        }
        printf("\n\n");
}


void map_test(void) 
{                  
        struct matrix_t *map = new_matrix(10, 10);

        int i;
        for (i=0; i<10; i++)
                mx_put(map, 5, i, 2);

        mx_put(map, 5, 3, 0);

        int startx = 0;
        int starty = 0;

        int goalx = 7;
        int goaly = 2;

        if (astar(map, 10, 10, starty, startx, goaly, goalx))
                printf("Got it!\n");
        else
                printf("Wtf?\n");
}


void peek_test(void)
{
        uint32_t a[4] = { 5934, 12, 9999, 13543 };
        int i;
        struct bh_t *bh = new_bh(4);

        for (i=0; i<4; i++)
                bh_add(bh, i, i, &a[i]);

        for (i=ROOT; i<bh->n; i++) {
                uint32_t *tmp = (uint32_t *)bh_peek(bh, i);
                if (tmp)
                        printf("i:%d a:%u\n", i, *tmp);
                else
                        printf("NULL\n");
        }
}


/*void membership_test(void)*/
/*{*/
        /*uint32_t a[4] = { 5934, 12, 9999, 13543 };*/
        /*uint32_t b[4] = { 12, 13, 1901, 5934 };*/
        /*int i;*/

        /*struct bh_t *bh = new_bh(4);*/
        /*printf("       +-------------------------------+\n");*/
        /*printf("Set a: | %5u | %5u | %5u | %5u |\n", a[0], a[1], a[2], a[3]);*/
        /*printf("       +-------------------------------+\n");*/
        /*printf("Set b: | %5u | %5u | %5u | %5u |\n", b[0], b[1], b[2], b[3]);*/
        /*printf("       +-------------------------------+\n");*/

        /*for (i=0; i<4; i++)*/
                /*bh_add(bh, i, i, &a[i]);*/

        /*for (i=0; i<4; i++) {*/
                /*if (member_of(bh, b[i]))*/
                        /*printf("b[%d] == %u is a member of a[]\n", i, b[i]);*/
                /*else*/
                        /*printf("b[%d] == %u is NOT a member of a[]\n", i, b[i]);*/
        /*}*/
/*}*/


void heap_test(void) 
{
        struct bh_t *mybh = new_bh(10);
        int i;

        char *foostring[10] = {"Good morning", "Breakfast cereal", "Noodle",
                               "Fine day", "Count the papers", "Bergamot aroma",
                               "Foreign components", "Scale exaggerated",
                               "Gjol-i-Nartes", "Deliberation" };

        for (i=0; i<10; i++) {
                uint32_t len = strlen(foostring[i]);
                if (bh_add(mybh, i, i, foostring[i]))
                        printf("Added foostring[%d] to heap\n", i);
                else
                        printf("Could not add foostring[%d] to heap!\n", i);
        }

        int j=0;
        while (mybh->node[j++]->key != 0);

        printf("root ends up at: %d\n", j);

        /*if (bh_setpri(mybh, 5, 9))*/
                /*printf("Changed priority\n");*/
        /*else*/
                /*printf("Didn't change priority\n");*/

        for (i=0; i<10; i++) {
                char *tmp;
                tmp = bh_pop(mybh);

                if (tmp) {
                        printf("Popped %d: %s\n", i, tmp);
                } else {
                        printf("Could not pop item %d!\n", i);
                }
        }
}
