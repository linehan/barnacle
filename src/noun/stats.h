#include "../com/arawak.h"

typedef uint32_t STATS;

void print_stats(STATS stat);

#define HP_COLOR  PUR_BRZ
#define HP_OFFSET (0) 
#define HP_MASK   (0x000000FFUL)
#define HP(stat)  ((stat & HP_MASK) >> HP_OFFSET)

#define SP_COLOR  PUR_DGREEN
#define SP_OFFSET (8) 
#define SP_MASK   (0x0000FF00UL)
#define SP(stat)  ((stat & SP_MASK) >> SP_OFFSET)

#define AP_COLOR  PUR_METALYELLOW
#define AP_OFFSET (16)
#define AP_MASK   (0x00FF0000UL)
#define AP(stat)  ((stat & AP_MASK) >> AP_OFFSET)
