/* BOAT NIBBLE GUIDE */
enum  boat_nibbles   {RIG,HDG,SAI,GUN,ROT,FLG,VEL};

enum hdg_t HDG_state;
enum grad_t VEL_state;

enum SAI_state_tags {                                                   
        POL, // Bare poles
        FOR, // Foresail
        MAI, // Mainsail
        MIZ, // Mizzen sail
        TPS, // Topsail
        LTG, // Lower topgallant
        UTG, // Upper topgalland
        ROY, // Royal sail
        JIB, // Jib
        SPA, // Spanker
        STU, // Studsail
        SPI, // Spinnaker
        KIT  // Kite
};

enum GUN_state_tags {
        HAS, // Vessel is equipped with cannon and they are operational.
        POW, // Vessel has powder availible.
        SHO, // Vessel has shot availible.
        LD0, // Loading stage 0
        LD1, // Loading stage 1
        RDY, // Loading complete (ready status)
        FIR, // Firing        ___________________________________
        ROU, // Round shot              XOR 0x8 with these to get 
        CAN, // Canister shot           the graphics indexes
        GRA, // Grape shot
        CHI, // Chain shot
        LAN, // Langrage (bags of junk)
        HOT, // Heated shot
        DBL, // Double shot
        EXP  // Exploding shot
};

enum boat_hull_gfx { HUL_N, HUL_E, HUL_S, HUL_W };

enum boat_mast_gfx { MAS_I, MAS_J, MAS_L };

enum boat_sail_gfx {
        SAI_DR, /* Down-right facing sail */ 
        SAI_DL, /* Down-left facing sail  */ 
        SAI_UR, /* Up-right facing sail   */ 
        SAI_UL, /* Up-left facing sail    */ 
        SAI_PI, /* Pinched sail           */ 
        SAI_SW, /* Swelling sail          */ 
        SAI_WL, /* Left-swelling sail     */ 
        SAI_WR  /* Right-swelling sail    */ 
};
