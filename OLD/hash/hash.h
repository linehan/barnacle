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
 * instruction sequence that I thought might work by interleaving two 16 bit 
 * words, then wrote a program to search for parameters which gave the greatest 
 * amount of avalanching before requiring a fix up for the end. I then added 
 * instructions that would be equivalent of unrolling the inner loop 
 * corresponding to padding the input with a fixed number zeros, then scanned 
 * for the set of parameters which could complete the avalanching for all the 
 * real input bits.
 *
 * I was shocked to find that there were no significant impediments to this 
 * exercise, and I easily found a hash function with all these properties after 
 * a few hours or work. I then subjected all realistic sub-bit patterns of the 
 * hash output to a simple statistical test and verified that it had a 
 * distribution equivalent to a uniformly random map.
 *
 * The moment of truth came with the performance test -- but given the 
 * architecture, it was a forgone conclusion. My hash function performs around 
 * 66% faster than Bob Jenkin's functions tested with various compilers.
 *                                                                      
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

static inline uint32_t fasthash(const char *data, size_t len) 
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
/******************************************************************************
 * Each entry in the hash table is contained in a HASHNODE, which stores the
 * string being indexed, the hash value, the 'key' in this case, i.e., the
 * piece of data which is checked during traversal to determine if a match has
 * been made or not, and a callback function pointer, which is set to some
 * callback function when the HASHNODE is initialized. It is this callback
 * that is the thing being "looked up", using the "key".
 ******************************************************************************/
typedef struct hash_node {
        char *string;
        uint32_t key;
        int nargs;
        void *data;
        void (*cb)(const void *);
        struct list_node node;
} HASHNODE;
/******************************************************************************
 * The hash table is a linked list of HASHNODE datatypes, stored in a data
 * structure of type HASHTABLE, which contains a pointer to the head of the
 * linked list, and a function pointer to the hash function (like a method).
 ******************************************************************************/
typedef struct hash_table {
        uint32_t size;
        struct list_head *tbl;
        uint32_t (*hash)(const char *ch, int len);
} HASHTABLE;

/******************************************************************************
 * Initialize a new HASHTABLE
 ******************************************************************************/
#define HASHTABLE_INIT(H) \
        init_new_hashtable(H)
static inline void init_new_hashtable(HASHTABLE *new)
{
        new->size = 0;
        new->hash = &fasthash;
        new->tbl = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->tbl);
}
/******************************************************************************
 * Initialize and append a new HASHNODE entry in the specified HASHTABLE 
 * structure.
 ******************************************************************************/
#define ADD_TO_HASHTABLE(H, str, data, cb) \
        hashtable_add(H, str, data, cb)
static inline void hashtable_add(HASHTABLE *H, const char *str, void *data, void *cb)
{
        HASHNODE *new = (HASHNODE *)malloc(sizeof(HASHNODE));

        /* Copy the reference string to the hash node. */
            int len = strlen(str);
        new->string = malloc(len * sizeof(char));
        strncpy(new->string, str, len);


        new->key = H->hash(str, len); /* Record the hash value */
        new->data = data;
        new->cb = cb;

        /* Add the new node to the HASHTABLE provided. */
        list_add(H->tbl, &(new->node));
}
/******************************************************************************
 * Traverse the HASHTABLE, comparing the hash of the pattern string with the
 * hash of each node's 'key' field. If a match is found, return a pointer to
 * that HASHNODE. If no match is found, return a NULL pointer.
 ******************************************************************************/
#define MATCH_HASH(H, string) \
        match_hash(H, string)
static inline HASHNODE *match_hash(HASHTABLE *H, const char *string)
{
        /* Compute the hash of the pattern string. */
        int len = strlen(string);
        uint32_t hash = H->hash(string, len);

        HASHNODE *tmp;
        /* Traverse the hashtable to find a match for the hash. */
        list_for_each(H->tbl, tmp, node) {
                if ((tmp->key == hash))
                        return tmp;
        }
        return NULL;
}
#endif /* See line 0 */
