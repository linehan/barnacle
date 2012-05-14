#include "../mob/mob.h"
#include "../noun/noun.h"


uint32_t ropekey;

void new_rope(void *self);
void new_torch(void *self);
void new_pickaxe(void *self);
void new_shovel(void *self);

void use_rope(void *self, struct noun_t *noun);
void use_pickaxe(void *self, struct noun_t *noun);
void use_torch(void *self, struct noun_t *noun);
void use_shovel(void *self, struct noun_t *noun);
void burn_torch(void *self, struct noun_t *noun);

