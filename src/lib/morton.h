#pragma once
#ifndef __MORTON_CODING
#define __MORTON_CODING

#include <stdint.h>

uint32_t mort(uint32_t y, uint32_t x);
uint32_t trom(uint32_t z);
uint32_t trom_x(uint32_t z);
uint32_t trom_y(uint32_t z);

#define MORT(y, x) mort((uint32_t)y, (uint32_t)x) 

#endif
