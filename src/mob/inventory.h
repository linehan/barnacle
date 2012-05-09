#include "../equip/items.h"

#define LH(mob) ((mob)->inv->lh)
#define RH(mob) ((mob)->inv->rh)

struct inventory_t {
        struct hashtable_t *tbl;
        uint32_t key[100];
        struct stdmenu_t *menu;
        int n;
        uint32_t lh;
        uint32_t rh;
        uint32_t ex;
        struct equip_t *tmp;
        void (*add)(struct equip_t *equip, struct mob_t *mob);
        void (*use)(uint32_t key, struct mob_t *mob);
        void (*get)(uint32_t key, struct mob_t *mob);
        void (*burn)(uint32_t key, struct mob_t *mob);
        void (*mkmenu)(struct mob_t *mob);
};


struct inventory_t *new_inventory(void);
