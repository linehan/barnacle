//enum map_nibs {LAY=0,ALT=1,BED=2,SED=3,SOI=4,MOI=5,TEM=6,WND=7};
//static const 
//char *map_nibs[8] = {"LAY","ALT","BED","SED","SOI","MOI","TEM","WND"};

#define LAYER_ENUM layer_enum
#define LAYER_CHAR layer_char
#define LAYER_LIST                                             \
        TWINDEX(XXX, 0) /* failsafe background (default)    */ \
        TWINDEX(BGR, 1) /* background                       */ \
        TWINDEX(HIG, 2) /* background highlights (animated) */ \
        TWINDEX(RIM, 3) /* edge effects (animated)          */ \
        TWINDEX(DRP, 4) /* the "drop" of the perspective    */ \
        TWINDEX(TOP, 5) /* the "top" of the perspective     */ \
        TWINDEX(GRO, 6) /* the ground                       */ \
        TWINDEX(HUT, 7) /* dwelling or structure            */ \
        TWINDEX(VEG, 8) /* vegetation                       */ \
        TWINDEX(SKY, 9) /* weather and atmospheric effects  */

#define ALTITUDE_ENUM altitude_enum
#define ALTITUDE_CHAR altitude_char
#define ALTITUDE_LIST           \
        TWINDEX(HIG4, 0)        \
        TWINDEX(HIG3, 1)        \
        TWINDEX(HIG2, 2)        \
        TWINDEX(HIG1, 3)        \
        TWINDEX(HIG0, 4)        \
        TWINDEX(LOW4, 5)        \
        TWINDEX(LOW3, 6)        \
        TWINDEX(LOW2, 7)        \
        TWINDEX(LOW1, 8)        \
        TWINDEX(LOW0, 9)        \
        TWINDEX(DROP, 10)       \
        TWINDEX(CLIF, 12)       
        
#define BEDROCK_ENUM bedrock_enum
#define BEDROCK_CHAR bedrock_char
#define BEDROCK_LIST            \
        TWINDEX(GRAN, 0) /* granite       */ \
        TWINDEX(SCHI, 1) /* schist        */ \
        TWINDEX(GABB, 2) /* gabbro        */ \
        TWINDEX(BASA, 3) /* basalt        */ \
        TWINDEX(ANDE, 4) /* andesite      */ \
        TWINDEX(DIOR, 5) /* diorite       */ \
        TWINDEX(HORN, 6) /* hornblende    */ \
        TWINDEX(TUFF, 7) /* volcanic tuff */ \
        TWINDEX(MARB, 8) /* marble        */ \
        TWINDEX(FELS, 9) /* felsite       */       


#define SEDIMENT_ENUM sediment_enum
#define SEDIMENT_CHAR sediment_char
#define SEDIMENT_LIST            \
        TWINDEX(BREC,  0) /* breccia    */ \
        TWINDEX(CHAL,  1) /* chalk      */ \
        TWINDEX(CHER,  2) /* chert      */ \
        TWINDEX(CLST,  3) /* claystone  */ \
        TWINDEX(DOLO,  4) /* dolomite   */ \
        TWINDEX(WACK,  5) /* greywacke  */ \
        TWINDEX(LIME,  6) /* limestone  */ \
        TWINDEX(MUDS,  7) /* mudstone   */ \
        TWINDEX(SHAL,  8) /* shale      */ \
        TWINDEX(SILS,  9) /* siltstone  */ \
        TWINDEX(TRAV, 10) /* travertine */ 

#define SOIL_ENUM soil_enum 
#define SOIL_CHAR soil_char
#define SOIL_LIST                                                                       \
        TWINDEX(ENTI,  1) /* entisol    - recently formed soil; unconsolidated       */ \
        TWINDEX(VRTI,  2) /* vertisol   - inverted soil; swell when wet, crack       */ \
        TWINDEX(INCP,  3) /* inceptisol - young soil; little eluviation/illuviation  */ \
        TWINDEX(ARID,  4) /* aridisol   - dry soil; scarce organics                  */ \
        TWINDEX(MOLL,  5) /* mollisol   - soft soil with thick A horizons            */ \
        TWINDEX(SPOD,  6) /* spodisol   - soil produced by podsolization; forests    */ \
        TWINDEX(ALFI,  7) /* alfisol    - soil with Al and Fe; clay accumulation     */ \
        TWINDEX(ULTI,  8) /* ultisol    - soil that are heavily leached              */ \
        TWINDEX(OXIS,  9) /* oxisol     - soil with heavy oxide content              */ \
        TWINDEX(HIST, 10) /* histosol   - organic soil                               */ \
        TWINDEX(ANDI, 11) /* andisol    - volcanic soil; high in glass content       */ \
        TWINDEX(GELI, 12) /* gelisol    - permafrost soil                            */

#define MOISTURE_ENUM moisture_enum 
#define MOISTURE_CHAR moisture_char
#define MOISTURE_LIST                   \
        TWINDEX(WET5,  0)               \
        TWINDEX(WET4,  1)               \
        TWINDEX(WET3,  2)               \
        TWINDEX(WET2,  3)               \
        TWINDEX(WET1,  4)               \
        TWINDEX(WET0,  5)               \
        TWINDEX(DRY5,  6)               \
        TWINDEX(DRY4,  7)               \
        TWINDEX(DRY3,  8)               \
        TWINDEX(DRY2,  9)               \
        TWINDEX(DRY1, 10)               \
        TWINDEX(DRY0, 11)               

#define TEMPERATURE_ENUM temperature_enum 
#define TEMPERATURE_CHAR temperature_char
#define TEMPERATURE_LIST                \
        TWINDEX(HI5,  0)                \
        TWINDEX(HI4,  1)                \
        TWINDEX(HI3,  2)                \
        TWINDEX(HI2,  3)                \
        TWINDEX(HI1,  4)                \
        TWINDEX(HI0,  5)                \
        TWINDEX(LO5,  6)                \
        TWINDEX(LO4,  7)                \
        TWINDEX(LO3,  8)                \
        TWINDEX(LO2,  9)                \
        TWINDEX(LO1, 10)                \
        TWINDEX(NIL, 11)                \
        TWINDEX(ALL, 11)               

#define HEADING_ENUM heading_enum 
#define HEADING_CHAR heading_char 
#define HEADING_LIST                     \
        TWINDEX(NORTH, 0)                \
        TWINDEX(NNE,   1)                \
        TWINDEX(NE,    2)                \
        TWINDEX(ENE,   3)                \
        TWINDEX(EAST,  4)                \
        TWINDEX(ESE,   5)                \
        TWINDEX(SE,    6)                \
        TWINDEX(SSE,   7)                \
        TWINDEX(SOUTH, 8)                \
        TWINDEX(SSW,   9)                \
        TWINDEX(SW,   10)                \
        TWINDEX(WSW,  11)                \
        TWINDEX(WEST, 12)                \
        TWINDEX(WNW,  13)                \
        TWINDEX(NW,   14)                \
        TWINDEX(NNW,  15)                


#define GLOSS \
        layer_char, altitude_char, bedrock_char, sediment_char, soil_char, \
        moisture_char, temperature_char, heading_char
