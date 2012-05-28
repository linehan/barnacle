/* 
 * fifo.c -- FIFO queue structures 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "fifo.h"
/*
      ,gPPRg,               
     dP'   `Yb    FIFO Queues / Circular Buffers
     8)     (8  
     Yb     dP    A little set of library routines for common operations
      "8ggg8"     on a circular, fixed-width buffer. The implementation
                  is first-in, first-out (FIFO). They are surprisingly 
                  and often elegantly applicable to a great many things.
*/




/*
 * FIFO enqueue -- write a value v to the circular buffer
 * @f: pointer to an allocated struct fifo_t
 * @v: the value to be enqueued
 */
void fifo_enq(struct fifo_t *f, uint32_t v)
{
        f->fifo[f->wr] = v;
        f->wr = (f->wr + 1) % f->len;
}




/*
 * FIFO dequeue -- read a value v from the circular buffer
 * @f: pointer to an allocated struct fifo_t
 */
uint32_t fifo_deq(struct fifo_t *f)
{
        uint32_t v;
        
        while (f->rd != f->wr)
        {
                v = f->fifo[f->rd];
                f->rd = (f->rd + 1) % f->len;
        }
        return (v);
}




/*
 * FIFO readn -- return the value n words backwards from the write pointer
 * @f: pointer to an allocated struct fifo_t
 * @n: number of words behind the write pointer
 */
uint32_t fifo_rdn(struct fifo_t *f, int n)
{
        return (f->fifo[((f->rd + n) % f->len)]);
}




/*
 * FIFO resize -- shrink the buffer from FIFO_BUFSIZE to something less
 * @f: pointer to an allocated struct fifo_t
 * @n: number of words in the buffer (should be < FIFO_BUFSIZE)
 */
uint32_t fifo_res(struct fifo_t *f, int n)
{
        f->len = (uint32_t)(n%FIFO_BUFSIZE);      
        if (f->len == 0) f->len = 1; // avoid floating-point exception
}




/*
 * FIFO dump -- write each value in the buffer to a destination p.
 * @f: pointer to an allocated struct fifo_t
 * @p: pointer to an allocated destination
 *
 * Example: 
 *
 *      bool spool; 
 *      uint32_t sum;
 *      uint32_t p;
 *       
 *      for (spool = fifo_dmp(&my->fifo, NULL);
 *           spool;
 *           spool = fifo_dmp(&my->fifo, &p)) 
 *      {
 *              sum += p;
 *      }
 */
bool fifo_dmp(struct fifo_t *f, uint32_t *p)
{
        static uint32_t i;

        if (p == NULL) i = f->wr; // First run
        else {
                *p = f->fifo[i];
                i = ((i+1) % f->len);
        }
        return (i==f->wr && p!=NULL) ? false : true ;
}




