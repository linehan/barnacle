#include "boat_model.h"

void boat_control(struct boat_t *boat, int order, int val);
void move_boat(struct boat_t *boat, int dir);
void sail_boat(struct boat_t *boat);
uint32_t change_course(struct boat_t *boat, int opt);
uint32_t turn_helm(struct boat_t *boat, int opt);
