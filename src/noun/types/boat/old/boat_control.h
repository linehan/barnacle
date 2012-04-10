#include "boat_model.h"

void boat_control(void *obj, int order, int val);
void move_boat(void *obj, int dir);
void sail_boat(void *obj);
uint32_t change_course(struct boat_t *boat, int opt);
uint32_t turn_helm(struct boat_t *boat, int opt);
