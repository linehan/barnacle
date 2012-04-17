#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ncurses.h>

#include "bh.h"



int main(void) 
{
        struct bh_t *mybh = new_bh(10);
        int i;

        char *foostring[10] = {"Good morning", "Breakfast cereal", "Noodle",
                               "Fine day", "Count the papers", "Bergamot aroma",
                               "Foreign components", "Scale exaggerated",
                               "Gjol-i-Nartes", "Deliberation" };


        for (i=0; i<10; i++) {
                if (bh_add(mybh, i, foostring[i]))
                        printf("Added foostring[%d] to heap\n", i);
                else
                        printf("Could not add foostring[%d] to heap!\n", i);
        }


        printf("Heap size: %d\nMax size: %d\n", mybh->n, mybh->max);



        for (i=0; i<10; i++) {
                char *tmp;
                tmp = bh_pop(mybh);

                if (tmp) {
                        printf("Popped %d: %s\n", i, tmp);
                        printf("Heap size: %d\nMax size: %d\n", mybh->n, mybh->max);
                } else {
                        printf("Could not pop item %d!\n", i);
                        printf("Heap size: %d\nMax size: %d\n", mybh->n, mybh->max);
                }
        }


        return 0;
}

