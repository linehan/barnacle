#ifndef __ENG_STATE
#define __ENG_STATE
#include "../lib/redblack/rb.h"





void set_state(struct rb_tree *tree, uint32_t key, int w, int n, int s);
int  get_state(struct rb_tree *tree, uint32_t key, int w, int n);
int   is_state(struct rb_tree *tree, uint32_t key, int w, int n, int s);
int   or_state(struct rb_tree *tree, uint32_t key, int w, int n, int s);





#define NNIBS    8  // Number of nibbles in a state word.
#define NSTATES 16  // Number of states each nibble can take.
#define NBITS   32  // Number of bits (total) in a state word.


/* Directional */
enum hdg_t {NORTH,NNE,NE,ENE,EAST,ESE,SE,SSE,SOUTH,SSW,SW,WSW,WEST,WNW,NW,NNW};        
static const char *hdg_tag[NSTATES] = {"NORTH","NNE","NE","ENE","EAST","ESE",
                                       "SE","SSE","SOUTH","SSW","SW","WSW",
                                       "WEST","WNW","NW", "NNW"};
/* Graduated */
enum grad_t {NIL=1,HI5=2,HI4=3,HI3=4,HI2=5,HI1=6,HI0=7,LO5=8,LO4=9,LO3=10,
             LO2=11,LO1=12,LO0=13,ALL=14};
static const char *grad_tag[NSTATES] = {"XXX","NIL","HI5","HI4","HI3","HI2",
                                        "HI1","HI0","LO5","LO4","LO3","LO2",
                                        "LO1","LO0","ALL"};
/* Stacking order */
enum lay_t {
        XXX = 0,  // failsafe background (default)
        BGR = 1,  // background
        HIG = 2,  // background highlights (animated)
        RIM = 3,  // edge effects (animated)
        DRP = 4,  // the "drop" of the perspective
        TOP = 5,  // the "top" of the perspective 
        GRO = 6,  // the ground
        HUT = 7,  // dwelling or structure
        VEG = 8,  // vegetation
        SKY = 9   // weather and atmospheric effects
};                                                                      

static const char *lay_tag[NSTATES] = {"XXX","BGR","HIG","RIM","DRP","TOP",
                                       "GRO","HUT","VEG","SKY","XXX","XXX",
                                       "XXX","XXX","XXX","XXX"};
/* Scalar value */
/* No enum is necessary, since it would be redundant. */
static const char *scal_tag[NSTATES] = {"XX","00","01","02","03","04","05",
                                        "06","07","08","09","10","11","12",
                                        "13","14"};


#endif
