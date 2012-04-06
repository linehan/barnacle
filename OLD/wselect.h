
typedef struct wch_selection {
        int n;
        int len;
        int xofs;
        int yofs;
        char curs;
        char pick;
        char focus;
        short pair[4];
        wchar_t wch[100];
        cchar_t cch[100];
        WINDOW *win;
        WINDOW *sub;
} WSELECT;

#define wselect_set_ofs(sel, y, x) \
        sel->yofs = y;             \
        sel->xofs = x 

#define wselect_set_win(sel, window) \
        sel->win = window

#define MINI(a,b) (((a)<(b)) ? (b) : (a))                                                            
#define MAXI(a,b) (((a)>(b)) ? (b) : (a)) 

enum wselect_driver_commands {
        SEL_FOCUS,
        SEL_UNFOCUS,
        SEL_FIRST,
        SEL_PREV,
        SEL_NEXT,
        SEL_SELECT,
        SEL_DESELECT,
        SEL_BEGINNING
};


/* Returns value of 'focus' member, which is 0 (false) or 1 (true) */
#define wselect_has_focus(sel) sel->focus

WSELECT *new_wselect(const wchar_t *selection, int n, short base, short hi, short sel, short selhi);
void wselect_paint(WSELECT *sel);
void wselect(WSELECT *sel);
void wselect_driver(WSELECT *sel, int command);
