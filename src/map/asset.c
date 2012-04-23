#include "../com/arawak.h"
#include "map.h"
#include "asset.h"


struct asset_t *new_asset(int a)
{
        struct asset_t *new;
        int i;
        int j;
        int idx;

        new = malloc(sizeof(struct asset_t));

        new->rec.h = _SANDHOLE_H;
        new->rec.w = _SANDHOLE_W;
        new->rec.l = _SANDHOLE_H * _SANDHOLE_W;
        new->win = newwin(_SANDHOLE_H, _SANDHOLE_W, 0, 0);
        new->pan = new_panel(new->win);

        new->cch = malloc(new->rec.l * sizeof(cchar_t));

        for (i=0; i<(new->rec.h); i++) {
        for (j=0; j<(new->rec.w); j++) {
                idx = (i*new->rec.w)+j;
                setcchar(&new->cch[idx], &_SANDHOLE_G[i][j], 0, _SANDHOLE_C[idx], NULL);
                mvwadd_wch(new->win, i, j, &new->cch[idx]);
        }
        }

        new->put = &put_asset;
        new->trigger = &fall_screen;

        return (new);
}



void put_asset(void *mymap, int idx, int y, int x)
{
        int i;
        int j;
        struct map_t *map = (struct map_t *)mymap;

        map->asset[idx]->rec.y = y;
        map->asset[idx]->rec.x = x;
        show_panel(map->asset[idx]->pan);
        move_panel(map->asset[idx]->pan, y, x);

        for (i=0; i<map->asset[idx]->rec.h-1; i++) {
        for (j=0; j<map->asset[idx]->rec.w; j++) {
                mx_set(map->hook, y+i, x+j, idx);
        }
        }
}


