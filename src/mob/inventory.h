#include "../equip/equipment.h"

#define inv_menu(mob) ((mob)->inv->menu)
#define inv_tmp(mob) ((mob)->inv->tmp)
#define inv_eqwin(mob) ((mob)->inv->equipped_win)

#define inv_add(eq,mob)   (mob)->inv->add(eq,(mob))
#define inv_use(key,mob)  (mob)->inv->use((key),(mob))
#define inv_get(key,mob)  (mob)->inv->get((key),(mob))
#define inv_burn(key,mob) (mob)->inv->burn((key),(mob))
#define inv_mkmenu(mob)   (mob)->inv->mkmenu((mob))

#define SET_CUR_KEY(mob) ((mob)->inv->cur_key = *(uint32_t *)inv_menu(mob)->cur_ptr)
#define CUR_KEY(mob)  ((mob)->inv->cur_key)
#define TORCHKEY(mob) ((mob)->inv->torch_key)
#define ROPEKEY(mob)  ((mob)->inv->rope_key)


struct inventory_t {
        struct hashtable_t *tbl;
        int n;
        uint32_t key[100];
        uint32_t cur_key;
        uint32_t torch_key;
        uint32_t rope_key;
        struct equip_t *tmp;
        struct stdmenu_t *menu;
        WINDOW *equipped_win;
        PANEL  *equipped_pan;
        void (*add)(struct equip_t *equip, struct mob_t *mob);
        void (*use)(uint32_t key, struct mob_t *mob);
        void (*get)(uint32_t key, struct mob_t *mob);
        void (*burn)(uint32_t key, struct mob_t *mob);
        void (*mkmenu)(struct mob_t *mob);
};


struct inventory_t *new_inventory(void);
