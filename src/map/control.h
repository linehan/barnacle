#ifndef __MAP_CONTROL_H
#define __MAP_CONTROL_H

struct map_t *new_zoom(struct map_t *map, int y, int x);
int map_control(int ch);
void update_map_control(void);
struct map_t *big_zoom(struct map_t *map);

#endif
