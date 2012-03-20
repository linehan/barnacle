#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include <stdbool.h>
#include <stdint.h>
#include "../verb/verb_model.h"
#include "../eng/model/bytes.h"
#include "../lib/redblack/rb.h"

struct noun_t {
        char *name;
        uint8_t job;
        uint32_t vitals;     // Byte-oriented
        uint32_t attributes; // Nibble-oriented
        struct verb_t verb;
};


struct noun_t *focused;
void focus(uint32_t key);
void unfocus(void);


// Holds keys of all creatures
uint32_t keyring[100];
void     install_key(uint32_t key, int option);
uint32_t request_key(int option);
enum install_key_opts {SUBJECT, OBJECT};


int nnouns(void);


void load_noun_test(void);
void new_noun(char *name, uint8_t job);


// Support modules
#include "deps/attributes.h"
#include "deps/jobs.h"
#include "deps/names.h"
#include "deps/vitals.h"

#endif
