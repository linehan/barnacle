extern cchar_t HERO;
extern cchar_t SURF0, SURF1, SURF2;
extern cchar_t SHORE, SAND;
extern cchar_t WSCK[8];
extern cchar_t FOREST[5];
extern cchar_t MTN[3];
extern cchar_t OCEAN[4];
extern cchar_t BLANK;
extern cchar_t BORDER;
extern cchar_t WARNBG, WARNSH, WARNPIP;
extern cchar_t PLAIN;

enum tiletype { __blank__ = 0,
                __ocean__ = 1
              };

void init_gfx_colors(void);
cchar_t *bg_tile(int type);
