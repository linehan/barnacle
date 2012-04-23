#pragma once
#ifndef __MAP_ASSET_H
#define __MAP_ASSET_H


struct asset_storage {
        int h;
        int w;
        short *color;
        wchar_t **wch;
};


struct asset_t {
        struct rec_t rec;
        WINDOW *win;
        PANEL  *pan;
        cchar_t *cch;
        void (*put)(void *map, int idx, int y, int x);
        void (*hide)(void *self);
        void (*show)(void *self);
        void (*trigger)(void);
};




struct asset_t *new_asset(int a);
void put_asset(void *mymap, int idx, int y, int x);



/* Assets
````````````````````````````````````````````````````````````````````````````` */


#define _SANDHOLE_H 3
#define _SANDHOLE_W 4
static short _SANDHOLE_C[]={DSAND,DSAND,DSAND,DSAND,
                            DSAND,ESAND,ESAND,DSAND,
                            CSAND,CSAND,CSAND,CSAND};
static wchar_t _SANDHOLE_G[3][4]={L"▛▀▀▜",
                                  L"▌▛▜▐",
                                  L"▔▔▔▔"};



#endif
