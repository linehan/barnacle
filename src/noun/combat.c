#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "noun.h"

inline int neighbors_exist(struct seed_t *seed)
{
        uint32_t *neighbor;
        int i;
        int n;

        for (i=0; i<8; i++) {
                neighbor = seed->nbr[i];
                if (neighbor)
                        n++;
        }
        return (n);
}

inline uint32_t random_neighbor(struct seed_t *seed)
{
        uint32_t *neighbor;
        int i;

        for (i=0; i<8; i++) {
                neighbor = seed->nbr[i];
                if (neighbor && *neighbor)
                        break;
        }

        return *neighbor;
}


uint32_t noun_nbr_any(struct noun_t *noun)
{
        struct seed_t seed; 
        mx_seed(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), &seed);

        if (neighbors_exist(&seed))
                return (random_neighbor(&seed));
        else
                return 0;
}

uint32_t noun_nbr(struct noun_t *noun, int dir)
{
        struct seed_t seed; 
        uint32_t *nbr;

        mx_seed(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), &seed);

        switch (dir) {
        case NORTH:
                nbr = seed.n;
                break;
        case SOUTH:
                nbr = seed.s;
                break;
        case EAST:
                nbr = seed.e;
                break;
        case WEST:
                nbr = seed.w;
                break;
        }
        return (*nbr);
}


