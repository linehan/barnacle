#include <stdio.h>
#include <stdlib.h>

struct eatme {
        int x;
        int y;
        double b;
        char ch;
        int xoo;
};

int main(void)
{
        int i;
        struct eatme *E = malloc(sizeof(struct eatme));
        struct eatme *F = malloc(sizeof(struct eatme));
        struct eatme *G = malloc(sizeof(struct eatme));


        E = &(struct eatme) {0, 3, 0.3, 'f', 920};
        printf("E:\nx: %d, y: %d, b: %g, ch: %c, xoo: %d\n\n", E->x, E->y, E->b, E->ch, E->xoo);
        E = &(struct eatme) {.ch='g'};
        G = &(struct eatme) {0};


        printf("E:\nx: %d, y: %d, b: %g, ch: %c, xoo: %d\n\n", E->x, E->y, E->b, E->ch, E->xoo);
        printf("G:\nx: %d, y: %d, b: %g, ch: %c, xoo: %d\n\n", G->x, G->y, G->b, G->ch, G->xoo);

        int ee = 0;
        for (i=0; i<162; i++) {
                printf("%d\n", i%123);
        }

        return 0;
}

