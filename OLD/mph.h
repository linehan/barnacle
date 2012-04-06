#pragma once
#ifndef __MPH_H
#define __MPH_H

struct mph_t {
        const char *fn;         // Filename where mph func will be located
        const char **keyring;   // Keys to the hashtable
        unsigned int n;         // Number of keys in keyring
        unsigned int z;         // Maximum size of keyring
};

struct mph_t *new_mph(const char *my_filename, unsigned int nkeys);
unsigned int mph_add(struct mph_t *tab, const char *key);
unsigned int mph_gen(struct mph_t *tab);
unsigned int mph_ind(struct mph_t *tab, const char *key);

#endif
