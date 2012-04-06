#include <ncurses.h>
#include <menu.h>

struct fancy_menu {
        MENU   *menu;
        WINDOW *detail_win;
        WINDOW *slider_win;
        const char **detail;
        cchar_t slider[6];
        short detail_color;
        short select_color;
};

void put_fancy_menu(struct fancy_menu *fancy, WINDOW *win, int h, int w, int y, int x);
struct fancy_menu *new_fancy_menu(const char **list, const char **desc, short fg, short bg);
void post_fancy_detail(struct fancy_menu *fancy);
void post_fancy_slider(struct fancy_menu *fancy);
void unpost_fancy_slider(struct fancy_menu *fancy);
void set_fancy_slider(struct fancy_menu *fancy, const wchar_t *slider_parts, short pair);
void tog_fancy_select(struct fancy_menu *fancy);
void set_fancy_select(struct fancy_menu *fancy, short pair);
void set_fancy_detail(struct fancy_menu *fancy, int h, int w, int y, int x, short pair);
void post_fancy_menu(struct fancy_menu *fancy);
