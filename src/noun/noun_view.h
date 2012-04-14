#include "../gfx/ui/stdmenu.h"

void *pattern_noun_menu(int op);
void close_noun_menu(int operand);
void open_noun_menu(int operand);
void tog_noun_menu(int operand);

MENU *get_noun_menu(int operand);
void list_nouns(int which, int opt);

enum list_noun_opts {ALL_NOUNS};

struct stdmenu_t *get_noun_struct(int op);

struct noun_t *current_noun(int op);
void focus_noun_menu(int op, bool opt);
bool isvis_noun_menu(int op);
