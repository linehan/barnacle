#pragma once
#ifndef __LIFO_H
#define __LIFO_H

#include <stdint.h>

#define LIFO_BUFSIZE 32

struct lifo_t {
        size_t size;
        uint32_t lifo[LIFO_BUFSIZE];
};


void lifo_push(struct lifo_t *l, uint32_t v);
uint32_t lifo_pop(struct lifo_t *l);


#endif
