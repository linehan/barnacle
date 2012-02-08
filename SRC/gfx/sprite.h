extern cchar_t HERO;
extern cchar_t SURF0, SURF1, SURF2;
extern cchar_t SHORE, SAND;
extern cchar_t WSCK[8];
extern cchar_t TREE[2];
extern cchar_t MTN[3];
extern cchar_t OCEAN[4];
extern cchar_t BLANK;
extern cchar_t BORDER;
extern cchar_t WARNBG, WARNSH, WARNPIP;
extern cchar_t PLAIN;
extern cchar_t DIRT[2];

enum tiletype { 
        __non__ = 0,
        __sea__ = 1,
        __san__ = 2,
        __mtn__ = 3
};

void init_gfx_colors(void);
cchar_t *bg_tile(int type);
cchar_t *get_tile(int type);
