#pragma once
#ifndef __GLOSS_EFFECTS_H
#define __GLOSS_EFFECTS_H

#define NSTYLES 6

enum gloss_styles { 
        REVEAL, 
        UNREVEAL, 
        SHINE, 
        PUSH_R, 
        PUSH_L,
        DROPOUT
};

void method_reveal(void *self, int L);
void method_unreveal(void *self, int L);
void method_shine(void *self, int L);
void method_pushr(void *self, int L);
void method_pushl(void *self, int L);
void method_dropout(void *self, int L);



struct cofam {
        short co;
        short hi;
        short hue[2];
};


enum schemes { 
        GLOSS_YELLOW   , GLOSS_ORANGE    , GLOSS_BEIGE       , GLOSS_BROWN ,
        GLOSS_LBLUE    , GLOSS_BLUE      , GLOSS_DBLUE       ,
        GLOSS_LGREEN   , GLOSS_GREEN     , GLOSS_DGREEN      ,
        GLOSS_LRED     , GLOSS_RED       , GLOSS_DRED        ,
        GLOSS_LSEA     , GLOSS_SEA       , GLOSS_DSEA        ,
        GLOSS_LWOOD    , GLOSS_WOOD      ,
        GLOSS_BLACK    , GLOSS_PURPLE    ,
        GLOSS_CERULEAN , GLOSS_CELESTIAL , GLOSS_METALYELLOW
};


static struct cofam colorfamily[]={
        {PUR_YELLOW      , PUR_BRONZE , { _PUR_YELLOW , __PUR_YELLOW }},
        {PUR_ORANGE      , PUR_BRONZE , { _PUR_ORANGE , __PUR_ORANGE }},

        {PUR_BEIGE       , PUR_WHITE  , { _PUR_BEIGE  , __PUR_BEIGE  }},
        {PUR_BROWN       , PUR_BEIGE  , { __PUR_BROWN , __PUR_BROWN  }},

        {PUR_LBLUE       , PUR_SKY    , { _PUR_LBLUE  , __PUR_LBLUE  }},
        {PUR_BLUE        , PUR_SKY    , { _PUR_BLUE   , __PUR_BLUE   }},
        {PUR_DBLUE       , PUR_LBLUE  , { _PUR_DBLUE  , __PUR_DBLUE  }},

        {PUR_LGREEN      , PUR_SKY    , { _PUR_LGREEN , __PUR_LGREEN }},
        {PUR_GREEN       , PUR_SKY    , { _PUR_GREEN  , __PUR_GREEN  }},
        {PUR_DGREEN      , PUR_SKY    , { _PUR_DGREEN , __PUR_DGREEN }},

        {PUR_LRED        , PUR_BRZ    , { _PUR_LRED   , __PUR_LRED   }},
        {PUR_RED         , PUR_BRZ    , { _PUR_RED    , __PUR_RED    }},
        {PUR_DRED        , PUR_BRZ    , { _PUR_DRED   , __PUR_DRED   }},

        {PUR_LSEA        , PUR_SKY    , { _PUR_LSEA   , __PUR_LSEA   }},
        {PUR_SEA         , PUR_SKY    , { _PUR_SEA    , __PUR_SEA    }},
        {PUR_DSEA        , PUR_SKY    , { _PUR_DSEA   , __PUR_DSEA   }},

        {PUR_LWOOD       , PUR_BRZ    , { _PUR_LWOOD  , __PUR_LWOOD  }},
        {PUR_WOOD        , PUR_BRZ    , { _PUR_WOOD   , __PUR_WOOD   }},

        {PUR_BLACK       , PUR_GREY   , { _PUR_GREY   , __PUR_GREY   }},
        {PUR_PURPLE      , PUR_SKY    , { _PUR_PURPLE , _PUR_PURPLE  }},

        {PUR_CERULEAN    , PUR_SKY    , { _PUR_DBLUE  , __PUR_DBLUE  }},
        {PUR_CELESTIAL   , PUR_SKY    , { _PUR_DBLUE  , __PUR_DBLUE  }},
        {PUR_METALYELLOW , PUR_BRZ    , { _PUR_YELLOW , __PUR_YELLOW }}
};


#define __GOLD  PUR_METALYELLOW, PUR_YELLOW
#define __SILV  PUR_WHITE, PUR_SKY
#define __WARN  PUR_WHITE, PUR_RED
#define __KILL  PUR_WHITE, PUR_RED

#define __HULK  PUR_SKY, PUR_LGREEN
#define __CERU  PUR_SKY, PUR_CERULEAN

#endif
