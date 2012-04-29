#include "../com/arawak.h"
#include "noun.h"

#define ALL_NOUNS 0

void make_nounmenu(int query);

MENU *get_nounmenu(void);
struct stdmenu_t *get_noun_struct(void);
struct noun_t *current_noun(void);

bool nounmenu_isvis(void);
void nounmenu_open(void);
void nounmenu_close(void);
void nounmenu_tog(void);
void nounmenu_focus(bool opt);

void nounmenu_print_highlight(void);
void nounmenu_print_name(short pair);
void nounmenu_print_vitals(void);
void nounmenu_print_attributes(void);

