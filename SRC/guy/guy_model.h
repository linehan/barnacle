#pragma once
#ifndef __GUY_MODEL
#define __GUY_MODEL

#include <stdbool.h>
#include <stdint.h>
#include "../eng/model/bytes.h"
#include "../lib/redblack/rb.h"

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
        bool forward;
        uint32_t vitals;     // Byte-oriented
        uint32_t attributes; // Nibble-oriented
        uint32_t action;     // Nibble-oriented
};

struct guy_t *focused;

uint32_t keyring[100]; // Holds keys of all creatures

void focus(uint32_t key);
int nguys(void);

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



// Support modules
#include "deps/attributes.h"
#include "deps/jobs.h"
#include "deps/names.h"
#include "deps/vitals.h"

#endif
