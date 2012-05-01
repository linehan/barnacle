#pragma once
#ifndef __MAP_TAGS_H
#define __MAP_TAGS_H


#define NLAYERS 16  // Number of layers on the world map.

enum map_byte_tags {LAY=0, ALT=1};

enum hdg_t wind_direction; /* Wind direction */


/* Values for the layer byte */
enum maplabel_tags {
        XXX,    /* failsafe background (default) */
        OCN,    /**/
        DRP,    /* the "drop" of the perspective */
        TOP,    /* the "top" of the perspective */
        SHO,    /* the ground */
        BEA,    /* the ground */
        TTO,    /* treetop */
        TTR,    /* treetrunk */
        DOR,
};                                                                      

enum maplayer_tags {
        BGR,  // background
        RIM,  // edge effects (animated)
        HIG,
};

enum label_tags {
        DOOR_CAVE  = 17,   // door
        DOOR_SAND  = 18,
        CAVE_FLOOR = 19,
        CAVE_WALL  = 20,
        CAVE_BGR   = 21,
};



/* -------------------------------------------------------------------------- */




/* -------------------------------------------------------------------------- */


#endif
