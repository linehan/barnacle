
struct item_t *equipped;

WINDOW *equipped_win;
PANEL  *equipped_pan;

void inventory_mkmenu(void *inventory);
void inventory_to_menu(struct list_head *inv);
int inventory_menu_control(int input);
void update_inventory_menu(void);
