#include "../mob/mob.h"



uint32_t ropekey;

void new_rope(void *self);
void new_torch(void *self);
void new_pickaxe(void *self);
void new_shovel(void *self);

void use_rope(struct mob_t *mob, void *self);
void use_pickaxe(struct mob_t *mob, void *self);
void use_torch(struct mob_t *mob, void *self);
void use_shovel(struct mob_t *mob, void *self);
void burn_torch(struct mob_t *mob, void *self);

