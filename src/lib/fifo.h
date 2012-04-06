#pragma once
#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>
#include <stdbool.h>
#define FIFO_BUFSIZE 32 /* When FIFO_BUFSIZE is a power of 2, most compilers
                           will optimize away the % operations. */

struct fifo_t {
        uint32_t rd; /* read */
        uint32_t wr; /* write */
        uint32_t len;/* length % FIFO_BUFSIZE */
        uint32_t fifo[FIFO_BUFSIZE];
};


/* Initialize new struct fifo_t by setting them equal to FIFO_INIT */
static const struct fifo_t FIFO_INIT = { 0, 0, FIFO_BUFSIZE, {0} };


    void fifo_enq(struct fifo_t *f, uint32_t v);
uint32_t fifo_deq(struct fifo_t *f);
uint32_t fifo_rdn(struct fifo_t *f, int n);
uint32_t fifo_res(struct fifo_t *f, int n);
    bool fifo_dmp(struct fifo_t *f, uint32_t *p);


#endif
