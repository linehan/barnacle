extern cchar_t   OCEAN[4], _OCEAN[4], __OCEAN[4];
extern cchar_t     MTN[3],   _MTN[3],   __MTN[3];
extern cchar_t    TREE[2],  _TREE[2],  __TREE[2];
extern cchar_t   GRASS[5], _GRASS[5], __GRASS[5];

extern cchar_t WARNBG, WARNSH, WARNPIP;
extern cchar_t BLANK, PLAIN;
extern cchar_t DIRT[2];

extern cchar_t UNDERSEA;
extern cchar_t DAYSKY;
extern cchar_t PURPLE[5];

enum tiletype { 
        __non__ = 0,
        __sea__ = 1,
        __san__ = 2,
        __mtn__ = 3,
        __gra__ = 4,
        __Gra__ = 5,
        __GRA__ = 6,
        __dirt__ = 7 
};

void init_gfx_colors(void);
cchar_t *bg_tile(int type);
cchar_t *get_tile(int type);
