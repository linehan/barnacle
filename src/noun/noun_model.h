#pragma once
#ifndef __NOUN_MODEL
#define __NOUN_MODEL

#include <stdbool.h>
#include <stdint.h>
#include "../verb/verb_model.h"
#include "../eng/model/bytes.h"
#include "../lib/redblack/rb.h"
#include "../mob/mob_model.h"

enum nountypes { SAILBOAT};

struct noun_t {
        char *name;
        uint32_t type;
        uint32_t job;
        uint32_t vitals;     
        uint32_t attributes; 
        struct verb_t verb;
        struct mob_t mob;
        void *obj;
        void (*render)(void *self);
        void (*modify)(void *self, int opt);
};


struct noun_t *focused;
void focus(uint32_t id);
void unfocus(void);

void *noun_obj(const char *name);
void noun_set_render(struct noun_t *noun, void (*func)(void *obj));
void noun_set_modify(struct noun_t *noun, void (*func)(void *obj, int opt));
void noun_render(struct noun_t *noun);
void noun_modify(struct noun_t *noun, int opt);

// Holds keys of all creatures
uint32_t keyring[100];
void     install_id(uint32_t id, int option);
uint32_t request_id(int option);
struct noun_t *get_noun(const char *name);
enum install_key_opts {SUBJECT, OBJECT};


extern int numnoun;


void load_noun_test(void);
void new_noun(const char *name, uint32_t type, uint32_t job, void *obj);


// Support modules
#include "deps/attributes.h"
#include "deps/jobs.h"
#include "deps/names.h"
#include "deps/vitals.h"

#endif
