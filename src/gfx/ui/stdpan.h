#include "../gfx.h"

void stdpan(WINDOW *win, WINDOW **sub, WINDOW **buf, PANEL **pan);

enum { DESC, MARK, POST };
void menu_wins(MENU *menu, WINDOW *win, WINDOW *sub);
void menu_look(MENU *menu, int option, bool setting, const char *ch);
void menu_pair(MENU *menu, short fore, short back);
void menu_control(MENU *menu, int esc);
MENU *make_new_menu(char **name, char **description, void **usrptr, int n);

