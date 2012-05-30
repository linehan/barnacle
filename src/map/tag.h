#pragma once
#ifndef __MAP_TAGS_H
#define __MAP_TAGS_H

/* Number of layers on the world map. */
#define NLAYERS 3  

enum map_byte_tags  {LAB, ALT};
enum map_layer_tags {BGR, RIM, HIG};

enum map_labels {
        XXX,    /* failsafe background (default) */
        OCN,    /* an ocean tile */
        DRP,    /* the "drop" of the perspective */
        TOP,    /* the "top" of the perspective */
        SHO,    /* the ground */
        BEA,    /* the ground */
        TTO,    /* treetop */
        TTR,    /* treetrunk */
        DOR,    /* door */
        CAVEDOOR_INSIDE,
        CAVEDOOR_OUTSIDE,
        CAVESOLID,
        CAVEFLOOR,
        CAVEWALL,
        CAVERUBBLE,
        ROPE,
        L_ROPE_ANCHOR,
        R_ROPE_ANCHOR,
        MEADOW,
};                                                                      

#endif
