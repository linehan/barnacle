// vim:fdm=marker
//{{{1 README
//##############################################################################
//#
//#   We think of the screen as a 2D grid of square tiles, with each tile
//#   being uniquely identified by a coordinate pair (y,x), where y is the
//#   row or line number, and x is the column number. 
//# 
//#   All information about a particular tile is stored in a single uint32_t,
//#   accessed by indexing a screen array. This array is linear, because we 
//#   generate a Morton number for each input pair (y,x), which serves as the
//#   index.
//#
//#   A "nibble" or "nybble" is a unit of 4 bits. There are 8 such nibbles in
//#   our uint32_t (32/4). Each nibble can take one of 2^4 (16) possible 
//#   states. Each nibble represents an option "category", and each of the
//#   16 states of a category represents a "setting". 
//#
//#   To keep myself from glazing over, each nibble is assigned a tag, which
//#   is composed of two complementary parts: an enum, and an array of labels
//#   which is indexed by each enum.
//#
//#   Likewise, all the possible states of a category are identified with an
//#   enum and corresponding label array.
//#
//#                  0000 0000 0000 0000 0000 0000 0000 0000                                
//#                  ENV  TER  SID  MOV  HDG  WND  WEA  ALT                                    
//#
//##############################################################################
//}}}1
#ifndef __ZBOX
#define __ZBOX

#include <stdint.h>

#define NNIBS    8  // number of nibbles in a MAP byte
#define NSTATES 16  // number of states each nibble can take
#define NBITS   32  // number of bits (total) in a MAP byte 

static const // Hexadecimal values of the 16 possible states of a MAP nibble
uint32_t state[NSTATES] = {0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x00000006, 0x00000007,
                           0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
                           0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F};

static const // Mask values for each of the 8 nibbles in a MAP byte
uint32_t scrub[NNIBS]   = {0x0FFFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, 0xFFF0FFFF,
                           0xFFFF0FFF, 0xFFFFF0FF, 0xFFFFFF0F, 0xFFFFFFF0};

static const // Inverted mask values for each of the 8 nibbles in a MAP byte
uint32_t gouge[NNIBS]   = {0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
                           0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F};

static const // The offset, in bits, of each of the 8 nibbles in a MAP byte
uint32_t offset[NNIBS] = {28, 24, 20, 16, 12, 8, 4, 0};



static const // Enumerated labels for each of the 8 nibbles in a MAP byte
char *nyb_tags[NNIBS] = {"LAY","ALT","BED","SED","SOI","MOI","TEM","HDG"};
           enum nibbles { LAY=0,ALT=1,BED=2,SED=3,SOI=4,MOI=5,TEM=6,HDG=7};



//{{{1 LAY -- layer classification 
//##############################################################################
//# These values represent the classification of a tile in the hierarchy of    #
//# the graphics stack. Rendering behaviors use this stacking order to         #
//# determine how to draw the screen, using the ncurses overlay() routine.     #
//##############################################################################
enum LAY_state_tags {
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
        //XX = 10, // reserved
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15, // reserved
};                                                                      
static const // Label strings for terrain states
char *LAY_tags[16] = {"XXX", "BGR", "HIG", "RIM", "DRP", "TOP", "GRO", "HUT",
                      "VEG", "SKY", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX"};
//}}}1
//}}}1
//{{{1 ALT -- altitude
enum ALT_state_tags {                                                   
        //XXX = 0, 
        HIG4 = 1,  // highgest
        HIG3 = 2, 
        HIG2 = 3, 
        HIG1 = 4,  
        HIG0 = 5, 
        LOW4 = 6,  // lowest 
        LOW3 = 7,   
        LOW2 = 8,  
        LOW1 = 9,  
        LOW0 = 10,
        DROP = 11, // a hole going down
        CLIF = 12  // a cliff face going up
};
static const 
char *ALT_tags[16] = {"XXXX", "HIG4", "HIG3", "HIG2", "HIG1", "HIG0", 
                      "LOW4", "LOW3", "LOW2", "LOW1", "LOW0", "DROP",
                      "CLIF", "XXXX", "XXXX"};
//}}}1
//{{{1 BED -- bedrock
enum BED_state_tags {
        //XXX = 0,
        GRAN = 1,  // granite
        SCHI = 2,  // schist
        GABB = 3,  // gabbro
        BASA = 4,  // basalt
        ANDE = 5,  // andesite
        DIOR = 6,  // diorite
        HORN = 7,  // hornblende
        TUFF = 8,  // volcanic tuff
        MARB = 9,  // marble
        FELS = 10  // felsite
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *BED_tags[16] = {"XXXX", "GRAN", "SCHI", "GABB", 
                      "BASA", "ANDE", "DIOR", "HORN", 
                      "TUFF", "MARB", "FELS", "XXXX", 
                      "XXXX", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 SED -- sedimentary/clastic rock
enum SED_state_tags {                                                   
        //XXX = 0,
        BREC = 1,  // breccia
        CHAL = 2,  // chalk
        CHER = 3,  // chert
        CLST = 4,  // claystone
        DOLO = 5,  // dolomite
        WACK = 6,  // greywacke
        LIME = 7,  // limestone
        MUDS = 8,  // mudstone
        SHAL = 9,  // shale
        SILS = 10, // siltstone
        TRAV = 11  // travertine
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *SED_tags[16] = {"XXXX", "BREC", "CHAL", "CHER", 
                      "CLST", "DOLO", "WACK", "LIME", 
                      "MUDS", "SHAL", "SILS", "TRAV", 
                      "XXXX", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 SOI -- topsoil
enum SOI_state_tags {                                                   
        //XXX = 0,
        ENTI = 1,  // entisol    - recently formed soil; unconsolidated
        VRTI = 2,  // vertisol   - inverted soil; swell when wet, crack
        INCP = 3,  // inceptisol - young soil; little eluviation/illuviation
        ARID = 4,  // aridisol   - dry soil; scarce organics
        MOLL = 5,  // mollisol   - soft soil with thick A horizons
        SPOD = 6,  // spodisol   - soil produced by podsolization; forests
        ALFI = 7,  // alfisol    - soil with Al and Fe; clay accumulation
        ULTI = 8,  // ultisol    - soil that are heavily leached
        OXIS = 9,  // oxisol     - soil with heavy oxide content
        HIST = 10, // histosol   - organic soil
        ANDI = 11, // andisol    - volcanic soil; high in glass content
        GELI = 12  // gelisol    - permafrost soil
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *SOI_tags[16] = {"XXXX", "ENTI", "VRTI", "INCP", 
                      "ARID", "MOLL", "SPOD", "ALFI", 
                      "ULTI", "OXIS", "HIST", "ANDI", 
                      "GELI", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 MOI -- moisture content
enum MOI_state_tags {                                                   
        WET5 = 1,  // top
        WET4 = 2,  // bottom
        WET3 = 3,  // left
        WET2 = 4,  // right 
        WET1 = 5,  // right 
        WET0 = 6,  // front 
        DRY5 = 7,  // reserved
        DRY4 = 8,  // back 
        DRY3 = 9,  // reserved
        DRY2 = 10, // reserved
        DRY1 = 11, // reserved
        DRY0 = 12  // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *MOI_tags[16] = {"XXXX", "WET5", "WET4", "WET3", 
                      "WET2", "WET1", "WET0", "DRY5", 
                      "DRY4", "DRY3", "DRY2", "DRY1", 
                      "XXXX", "XXXX", "XXXX", "XXXX"};

//{{{1 TEM -- temperature
enum TEM_state_tags {                                           
        //XXX = 0,
        HOT5 = 1,  // very hot
        HOT4 = 2,  
        HOT3 = 3, 
        HOT2 = 4, 
        HOT1 = 5, 
        HOT0 = 6,
        COL5 = 7,  // very cold
        COL4 = 8,  
        COL3 = 9,  
        COL2 = 10, 
        COL1 = 11, 
        COL0 = 12
};
static const 
char *TEM_tags[16] = {"XXXX", "HOT5", "HOT4", "HOT3", "HOT2", "HOT1", "HOT0", 
                      "COL5", "COL4", "COL3", "COL2", "COL1", "COL0",
                      "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 HDG -- wind heading
enum HDG_state_tags {                                               
        // NOTE: WID (wind direction) and HDG (heading) use the same tags
        NORTH = 0,  // north
        NNE   = 1,  // north-north-east
        NE    = 2,  // north-east
        ENE   = 3,  // east-north-east
        EAST  = 4,  // east
        ESE   = 5,  // east-south-east 
        SE    = 6,  // south-east
        SSE   = 7,  // south-south-east
        SOUTH = 8,  // south
        SSW   = 9,  // south-south-west
        SW    = 10, // south-west
        WSW   = 11, // west-south-west
        WEST  = 12, // west
        WNW   = 13, // west-north-west
        NW    = 14, // north-west
        NNW   = 15  // north-north-west
}; 
static const 
char *HDG_tags[16] = {"NORTH", "NNE", "NE", "ENE", "EAST" , "ESE", "SE", "SSE", 
                      "SOUTH", "SSW", "SW", "WSW", "WEST" , "WNW", "NW", "NNW"};
//}}}1


static const // Arrays of tag values for each of the 8 nibbles in a MAP byte
char **tags[8] = { LAY_tags, ALT_tags, BED_tags, SED_tags, 
                   SOI_tags, MOI_tags, TEM_tags, HDG_tags };



//##############################################################################
//# The ZBOX data type contains an array of uint32_t's, whose length is the    #
//# number of individual tiles that can be represented on-screen as a pair     #
//# of coordinates (y,x), according to their Morton number, or z-code.         #
//##############################################################################
typedef struct screen_matrix {
        uint32_t *box;
        uint32_t max;
        uint32_t z;
} ZBOX;



//##############################################################################
// Compute the Morton number of (y,x), and store it in z
//##############################################################################
inline static void mort(uint32_t y, uint32_t x, uint32_t *z)
{
        static const uint32_t B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
        static const uint32_t S[] = {1, 2, 4, 8};

        x = (x | (x << S[3])) & B[3];
        x = (x | (x << S[2])) & B[2];
        x = (x | (x << S[1])) & B[1];
        x = (x | (x << S[0])) & B[0];
        
        y = (y | (y << S[3])) & B[3];
        y = (y | (y << S[2])) & B[2];
        y = (y | (y << S[1])) & B[1];
        y = (y | (y << S[0])) & B[0];

        *z = x | (y << 1);
}

#endif
//##############################################################################
// PROTOTYPES
//##############################################################################
ZBOX *new_zbox(uint32_t rows, uint32_t cols);
inline void _set_nyb(uint32_t *B, int n, int s);
int  _is_nyb(uint32_t B, int n, int s);
const char *_get_nybtag(uint32_t B, int n);
void _stat_nyb(uint32_t B);
