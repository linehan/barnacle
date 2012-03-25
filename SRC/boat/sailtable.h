////////////////////////////////////////////////////////////////////////////////
/*
  The sail state lookup table is used to decide the appropriate sail
  graphic to be drawn, and is indexed by the heading and the wind direction.
*/
static unsigned char stbl[16][16] = {
/* N       NNE     NE      ENE     E       ESE     SE      SSE  
   S       SSW     SW      WSW     W       WNW     NW      NNW
   ---------------------------------------------------------------------------*/
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*N*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NNE*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NE*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
/* --------------------------------------------------------------             */
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ENE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*E*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ESE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*SE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
/* --------------------------------------------------------------             */
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSE*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*S*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
/* --------------------------------------------------------------             */
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*WSW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*W*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*WNW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*NW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
/* --------------------------------------------------------------             */
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NNW*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI}};
/* ---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
/*
  The mast state lookup table is similar to the sail table. There are
  some minor differences, but they are indexed the same.
*/
static unsigned char mtbl[16][16] = {
/* N       NNE     NE      ENE     E       ESE     SE      SSE  
   S       SSW     SW      WSW     W       WNW     NW      NNW
   ---------------------------------------------------------------------------*/
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*N*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NNE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
/* -------------------------------------------------------------              */
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*ENE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*E*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*ESE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*SE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
/* -------------------------------------------------------------              */
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SSE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*S*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SSW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
/* -------------------------------------------------------------              */
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*WSW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*W*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*WNW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*NW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
/* -------------------------------------------------------------              */
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NNW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  }};
/* ---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////



