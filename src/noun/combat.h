#pragma once
#ifndef __NOUN_COMBAT_H
#define __NOUN_COMBAT_H

#include "../com/barnacle.h"

uint32_t noun_nbr(struct noun_t *noun, int dir);
uint32_t noun_nbr_any(struct noun_t *noun);
uint32_t noun_nbr_model(struct noun_t *noun, int dir, int model);

#endif
