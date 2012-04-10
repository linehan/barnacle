#pragma once
#ifndef __ATTRIBUTES_MODEL
#define __ATTRIBUTES_MODEL
#include "../noun_model.h"

/*
  Creatures have an 'attributes' member, which is a packaged state consisting
  of 8 nibbles of a 32-bit uint32_t. They will be familiar to anybody with a
  passing acquaintence with D&D.
*/
enum attribute_enum {STR,DEX,VIT,AGI,INT,WIS,CHA,LUC};
static const wchar_t attr_symbols[]={L"ΣΦΔAΨWΧΛ"};

static inline uint32_t get_attribute(int id, int n)
{
        focus(id);
        return (get_nibble(focused->attributes, n));
}

static inline void set_attribute(int id, int n, int s)
{
        focus(id);
        set_nibble(&focused->attributes, n, s);
}

static inline void unpack_attributes(int id, int *dest)
{
        focus(id);
        unpack_nibbles(focused->attributes, dest, NNIBBLES);
}

#endif
