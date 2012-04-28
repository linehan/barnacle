#pragma once
#ifndef __NOUN_MODELS_H
#define __NOUN_MODELS_H
#include "noun.h"


struct animation *animation_type;

MODIFY_METHOD modify[100];
RENDER_METHOD render[100];

void apply_noun_model(struct noun_t *noun);


#endif
