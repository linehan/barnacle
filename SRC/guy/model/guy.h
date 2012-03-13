#ifndef __CREATURES
#define __CREATURES



#include "../../eng/model/bytes.h"
#include "../../lib/redblack/rb.h"



struct guy_t {
        char fname[30];
        char mname[30];
        char lname[30];
        char birth[30];
        uint8_t job;
        uint8_t age;
        uint8_t weight;
        uint8_t height;
        uint8_t gender;
        uint8_t xpulse;
        bool positive;
        uint32_t vitals;      // Byte-oriented
        uint32_t attributes;  // Nibble-oriented
        uint32_t action;      // Nibble-oriented
};



struct rb_tree *GUY_TREE; // Holds all creatures
uint32_t muster[100];     // Holds keys of all creatures


static struct guy_t *focused;
static inline void focus(uint32_t key)
{
        focused = (struct guy_t *)rb_extra(GUY_TREE, key);
}


// Return number of creatures in GUY_TREE
static inline int nguys(void)
{
        if (GUY_TREE == NULL)   return 0;
        else                    return (GUY_TREE->n);
}



void load_guy_test(void);

uint32_t new_guy(char *firstname,
                 char *middlename,
                 char *lastname,
                 char *birthplace,
                 uint8_t job,
                 uint8_t age,
                 uint8_t weight,
                 uint8_t height,
                 uint8_t gender);

#endif
