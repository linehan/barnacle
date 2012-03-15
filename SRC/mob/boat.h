<<<<<<< .merge_file_g1dzWx
// vim:fdm=marker
#ifndef __BOAT_OPTIONS
#define __BOAT_OPTIONS
#include "../eng/state.h"

//{{{1 BOAT NIBBLE GUIDE 
static const 
char *boat_nibs[8] = {"LAY","RIG","HDG","SAI","GUN","ROT","FLG","VEL"};
enum  boat_nibbles   {       RIG=1,HDG=2,SAI=3,GUN=4,ROT=5,FLG=6,VEL=7};
//}}}1
//{{{1 LAY -- layer classification 
enum lay_t boat_layer;
//}}}1
//{{{1 RIG -- rig or type of vessel
enum RIG_state_tags {                                           
        DING = 1,
        PINN = 2,
        CANO = 3,
        BARG = 4,
        KNAR = 5,
        SLOO = 6,
        CUTT = 7,
        YAWL = 8,
        SCHO = 9,
        BRIG = 10,
        BARQ = 11,
        FULL = 12,
        BRAG = 13,
        XEBE = 14,
        JUNK = 15 
};
static const 
char *RIG_tag[16] = {"XXXX", "DING", "PINN", "CANO", "BARG", "KNAR", "SLOO", 
                     "CUTT", "YAWL", "SCHO", "BRIG", "BARQ", "FULL", "BRAG",
                     "XEBE", "JUNK"};
//}}}1
//{{{1 HDG -- ship heading (directional)
enum hdg_t HDG_state;
//}}}1
//{{{1  SAI -- sail status
enum SAI_state_tags {                                                   
        //XXX = 0, 
        POL = 1,  // Bare poles
        FOR = 2,  // Foresail
        MAI = 3,  // Mainsail
        MIZ = 4,  // Mizzen sail
        TPS = 6,  // Topsail
        LTG = 7,  // Lower topgallant
        UTG = 8,  // Upper topgalland
        ROY = 9,  // Royal sail
        JIB = 5,  // Jib
        SPA = 10, // Spanker
        STU = 11, // Studsail
        SPI = 12, // Spinnaker
        KIT = 13  // Kite
};
static const 
char *SAI_tag[16] = {"XXX", "POL", "FOR", "MAI", "MIZ", "TOP", "TPS", "LTG", 
                     "UTG", "ROY", "JIB", "SPA", "STU", "SPI", "KIT", "XXX"};
//}}}1
//{{{1 GUN -- gun status
enum GUN_state_tags {
        //XXX = 0,
        HAS = 1,  // Vessel is equipped with cannon and they are operational.
        POW = 2,  // Vessel has powder availible.
        SHO = 3,  // Vessel has shot availible.
        LD0 = 4,  // Loading stage 0
        LD1 = 5,  // Loading stage 1
        RDY = 6,  // Loading complete (ready status)
        FIR = 7,  // Firing        ___________________________________
        ROU = 8,  // Round shot              XOR 0x8 with these to get 
        CAN = 9,  // Canister shot           the graphics indexes
        GRA = 10, // Grape shot
        CHI = 11, // Chain shot
        LAN = 12, // Langrage (bags of junk)
        HOT = 13, // Heated shot
        DBL = 14, // Double shot
        EXP = 15  // Exploding shot
};
static const 
char *GUN_tag[16] = {"XXX", "HAS", "POW", "SHO", "LD0", "LD1", "LD2", "RDY",
                     "ROU", "CAN", "GRA", "CHA", "LAN", "HOT", "DBL", "EXP"};
//}}}1
//{{{1 ROT -- rot and decay 
enum ROT_state_tags {                                                   
        //XXX = 0,
        FRE = 1,
        WRM = 2,
        TER = 3,
        BAR = 4
};
static const 
char *ROT_tag[16] = {"XXX", "FRE", "WRM", "TER", "BAR", "XXX", "XXX", "XXX", 
                     "XXX", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX"};
//}}}1
//{{{1 FLG -- flags and signals
enum FLG_state_tags {                                                   
        //XXX = 0,
        SUR = 1,
        HAI = 2,
        PIR = 3
};
static const 
char *FLG_tag[16] = {"XXX", "SUR", "HAI", "PIR", "XXX", "XXX", "XXX", "XXX",
                     "XXX", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX"};
//}}}1
//{{{1 VEL -- velocity (graduated)
enum grad_t VEL_state;
//}}}1
//{{{1 OPTIONS INDEX
static const
char **boat_opts[8] = {lay_tag,RIG_tag,hdg_tag,SAI_tag,GUN_tag,ROT_tag,FLG_tag,grad_tag};
//}}}1

extern struct mob_t *_BOAT;
extern struct rb_node *BOAT_NODE;
extern struct rb_tree *BOAT_TREE;

uint32_t new_boat(struct map_t *map);
void init_boats(void);
void nominate_boat(uint32_t key);
void sync_boat(void);
void *sail_boat(void *);
void order_boat(int, int);

#endif


=======
int init_boat(void);

void nominate_boat(MOB *mob);

MOB *new_boat();

void sync_boat(void);

int get_boat(MOB *mob, int a, int b);
int set_boat(MOB *mob, int a, int b, int val);

MOB *_BOAT;
void *sail_boat(void *);

void order_boat(int, int);

int get_pointofsail(void);



void deck_init(void);

void draw_deck(void);

void tog_deck(void);

enum boat_options {
        __now__ = 0, 
        __req__ = 1, 
        __buf__ = 2, 

        __hdg__ = 3, 
        __rig__ = 4, 
        __anc__ = 5
};
>>>>>>> .merge_file_wqThWu
