#pragma once
#ifndef __DOOR_H
#define __DOOR_H

enum door_tag { CAVE_DOOR };

struct hashtable_t *doortbl;

struct door_t {
        uint32_t id;
        enum door_tag tag;
        struct map_t *src;
        struct map_t *dest;
        struct rec_t rec;
        void (*trigger)(void *self);
};


void put_door(int tag, struct map_t *src, struct map_t *dest, int h, int w, int y, int x);
struct door_t *get_door(uint32_t key);
void door_trigger(void (*trigger)(void *self), struct door_t *door);
void place_door_tile(struct map_t *map, int y, int x);




struct asset_t *new_asset(int a);
void put_asset(void *mymap, int idx, int y, int x);



/* Assets
````````````````````````````````````````````````````````````````````````````` */


//#define _SANDHOLE_H 3
//#define _SANDHOLE_W 4
//static short _SANDHOLE_C[]={DSAND,DSAND,DSAND,DSAND,
                            //DSAND,ESAND,ESAND,DSAND,
                            //CSAND,CSAND,CSAND,CSAND};
//static wchar_t _SANDHOLE_G[3][4]={L"▛▀▀▜",
                                  //L"▌▛▜▐",
                                  //L"▔▔▔▔"};



#endif
