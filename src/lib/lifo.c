#include <stdlib.h>
#include <stdio.h>
#include "lifo.h"

void lifo_push(struct lifo_t *l, uint32_t v)
{
        if (l->size == LIFO_BUFSIZE) {
                fputs("Error: stack overflow\n", stderr);
                abort();
        }
        else
                l->lifo[l->size++] = v;
}


uint32_t lifo_pop(struct lifo_t *l)
{
        if (l->size == 0) {
                fputs("Error: stack underflow\n", stderr);
                abort();
        }
        else
                return (l->lifo[--l->size]);
}
