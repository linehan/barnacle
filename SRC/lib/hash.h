#ifndef __HASHTABLE_ROUTINES
#define __HASHTABLE_ROUTINES
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "dep/list.h"
/******************************************************************************
 * Paul Hsieh's blazing fast hash function.                             
 * See http://www.azillionmonkeys.com/qed/hash.html
 *
 * If I was going to try to outperform Bob Jenkins' function, I would have to 
 * take a step back and understand the functional nature of the bottleneck in 
 * these functions. The functions other than Bob Jenkins' basically operated 
 * on the idea of consuming one 8-bit byte at a time of input data and mixing 
 * each in some injective way into some 32-bit accumulator which, after 
 * possible post processing, is simply output. One can see the motivation of 
 * this idea -- each input byte can be mixed twice with a large degree of 
 * freedom in the 32 bit accumulator without self overlapping. Thus in 
 * successive steps its only really required to sort of "spread out" 
 * consecutive sequences of at most 8 bits in such a way that previous bytes 
 * don't obviously cancell out.
 *
 * This is explicitely seen in the "One at a Time hash" function. In fact, for 
 * each byte only a few very simple operations are performed -- a final short 
 * sequence of operations is required at the end. These operations at the end 
 * are required to make sure the bits in the last few bytes fully "avalanche" 
 * to all the output bytes. Avalanching is the property between an input and 
 * output bit where the output bit will flip with a probability p ("close" to 
 * 0.5) if the input bit is flipped relative to any random input data. A good 
 * hash function requires avalanching from all input bits to all the output 
 * bits. (Incidentally, Bob Jenkins overly chastizes CRCs for their lack of 
 * avalanching -- CRCs are not supposed to be truncated to fewer bits as other 
 * more general hash functions are; you are supposed to construct a custom CRC 
 * for each number of bits you require.) 
 *
 * Using the One at a Time hash function as a model, the next obvious question 
 * to ask is "why not try to use fewer operations between data fragments"? The 
 * idea would be to rely more heavily on fixups at the end to produce the final 
 * avalanching which adds a constant time overhead in hopes of reducing the 
 * linear overhead. I.e., the mixing function would in fact operate much more 
 * slowly relative to the stream of input bytes, but this would not matter to 
 * the bulk of the early bytes because they would eventually reach a maximal 
 * point of avalanching anyway.
 *
 * So my thought was to use fewer instructions per input fragment and to 
 * increase the size of the input fragment from 8 bits to 16 bits. On the x86 
 * this latter idea has a particularly high impact on performance since these 
 * architectures have hardware support for unaligned 16 bit word accesses. 
 * Using Bob Jenkin's definition of avalanching, I chose an inner loop        
 ******************************************************************************/
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

static inline uint32_t fasthash(const char *data, int len) 
{
        uint32_t hash = len, tmp;
        int rem;

            if (len <= 0 || data == NULL) return 0;

            rem = len & 3;
            len >>= 2;

            /* Main loop */
            for (;len > 0; len--) {
                hash  += get16bits (data);
                tmp    = (get16bits (data+2) << 11) ^ hash;
                hash   = (hash << 16) ^ tmp;
                data  += 2*sizeof (uint16_t);
                hash  += hash >> 11;
            }

            /* Handle end cases */
            switch (rem) {
                case 3: hash += get16bits (data);
                        hash ^= hash << 16;
                        hash ^= data[sizeof (uint16_t)] << 18;
                        hash += hash >> 11;
                        break;
                case 2: hash += get16bits (data);
                        hash ^= hash << 11;
                        hash += hash >> 17;
                        break;
                case 1: hash += *data;
                        hash ^= hash << 10;
                        hash += hash >> 1;
            }

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
}
