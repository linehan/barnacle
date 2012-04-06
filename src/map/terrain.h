#include "map.h"

void draw_layers(struct map_t *map, double **pmap);
void surface_flow(struct map_t *map);
void set_surface_flow(int n);
void dec_surface_flow(void);
void inc_surface_flow(void);
void init_surface_flow(int n);

