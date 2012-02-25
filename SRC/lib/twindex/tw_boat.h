
#define GUN_ENUM gun_enum
#define GUN_CHAR gun_char
#define GUN_LIST           \
        TWINDEX(ROU_F,  0) \
        TWINDEX(CAN_F,  1) \
        TWINDEX(GRA_F,  2) \
        TWINDEX(CHA_F,  3) \
        TWINDEX(LAN_F,  4) \
        TWINDEX(HOT_F,  5) \
        TWINDEX(DBL_F,  6) \
        TWINDEX(EXP_F,  7) \
        TWINDEX(ROU_S,  8) \
        TWINDEX(CAN_S,  9) \
        TWINDEX(GRA_S, 10) \
        TWINDEX(CHA_S, 11) \
        TWINDEX(LAN_S, 12) \
        TWINDEX(HOT_S, 13) \
        TWINDEX(DBL_S, 14) \
        TWINDEX(EXP_S, 15) 

#define HULL_ENUM hull_enum
#define HULL_CHAR hull_char
#define HULL_LIST                                  \
        TWINDEX(HUL_N, 0)  /* Up-facing hull    */ \
        TWINDEX(HUL_E, 1)  /* Right-facing hull */ \
        TWINDEX(HUL_S, 2)  /* Down-facing hull  */ \
        TWINDEX(HUL_W, 3)  /* Left-facing hull  */ 

#define ANCHOR_ENUM anchor_enum
#define ANCHOR_CHAR anchor_char
#define ANCHOR_LIST                                 \
        TWINDEX(ANC_D, 0)  /* Anchor dropped     */ \
        TWINDEX(ANC_S, 1)  /* Sea anchor dropped */ 

#define MAST_ENUM mast_enum
#define MAST_CHAR mast_char
#define MAST_LIST                                \
        TWINDEX(MAS_I, 0)  /* "I" shaped mast */ \
        TWINDEX(MAS_J, 1)  /* "J" shaped mast */ \
        TWINDEX(MAS_L, 2)  /* "L" shaped mast */ 

#define SAIL_ENUM sail_enum
#define SAIL_CHAR sail_char
#define SAIL_LIST                                       \
        TWINDEX(SAI_DR, 0) /* Down-right facing sail */ \
        TWINDEX(SAI_DL, 1) /* Down-left facing sail  */ \
        TWINDEX(SAI_UR, 2) /* Up-right facing sail   */ \
        TWINDEX(SAI_UL, 3) /* Up-left facing sail    */ \
        TWINDEX(SAI_PI, 4) /* Pinched sail           */ \
        TWINDEX(SAI_SW, 5) /* Swelling sail          */ \
        TWINDEX(SAI_WL, 6) /* Left-swelling sail     */ \
        TWINDEX(SAI_WR, 7) /* Right-swelling sail    */ 

#define GLOSS \
        gun_char, hull_char, anchor_char, mast_char, sail_char
