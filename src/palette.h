/* FG_BG color pairs; odd ones are swaps of even ones. */
enum ramp_pairs {
  BLACK_DGREY   = 10,
  DGREY_BLACK   = 11,
  DGREY_GREY    = 12,
  GREY_DGREY    = 13,
  GREY_LGREY    = 14,
  LGREY_GREY    = 15,

  DGREY_DBLUE   = 16,
  DBLUE_DGREY   = 17,
  DBLUE_BLUE    = 18,
  BLUE_DBLUE    = 19,
  BLUE_LBLUE    = 20,
  LBLUE_BLUE    = 21,
  LBLUE_LGREY   = 22,
  LGREY_LBLUE   = 23,

  DGREY_DGREEN  = 24,
  DGREEN_DGREY  = 25,
  DGREEN_GREEN  = 26,
  GREEN_DGREEN  = 27,
  GREEN_LGREEN  = 28,
  LGREEN_GREEN  = 29,
  LGREEN_YELLOW = 30,
  YELLOW_LGREEN = 31,
  YELLOW_LGREY  = 32,
  LGREY_YELLOW  = 33,

  DGREY_BROWN   = 34,
  BROWN_DGREY   = 35,
  BROWN_ORANGE  = 36,
  ORANGE_BROWN  = 37,
  ORANGE_YELLOW = 38,
  YELLOW_ORANGE = 39,

  DGREY_RED     = 40,
  RED_DGREY     = 41,
  RED_PINK      = 42,
  PINK_RED      = 43,
  PINK_BEIGE    = 44,
  BEIGE_PINK    = 45,
  BEIGE_YELLOW  = 46,
  YELLOW_BEIGE  = 47,

  SEA_DEEP    = 48,
  SEA_SURF    = 49,
  SEA_SHORE   = 50,
  BOAT_DEEP   = 51,
  SAIL_DEEP   = 52,
};

int init_palette(int);
void guy_bg_update(short);
