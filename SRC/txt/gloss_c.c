
#include "gloss.h"
/*
 * TEXT EFFECTS SHOULD HAVE 4 STAGES
 *      0. Appear 
 *      1. Stabilize
 *      2. Decay
 *      3. Disappear
 *
 *      Each of these stages has a number of loops to id its duration,
 *      which may also receive their input from a randomizer.
 *
 *      You pass say() an option which is switched to set a mode. Once
 *      the mode is set, it must go to completion unless it is interrupted.
 *      The mode will proceed from 0...3, and release the mode lock after
 *      it has completed.
 */
bool say(struct gloss_t *test)
{
        /*#define SKIP_LOOP(x) if ((SKIP++ % (x))!=0) return false*/
        #define ON_LOOP(x)      if ((LOOP++) == (x))
        #define  REPORT(x)      STATUS=(x); break
        #define   YIELD(x)      LOOP=0, STAGE=(x)
        #define     UNLOOP      STAGE=LOOP=0; return true

        #define LOOPVAR(v) v = (LOOP)

        static unsigned int LOOP;
        static unsigned int STAGE;
        static unsigned int SKIP;
        static bool STATUS;
        int L;

        LOOPVAR(L);
        if (test->wcs == NULL) return false;

        switch (STAGE)
        {
        case 0: ON_LOOP(test->len) YIELD(1);
                test->put[REVEAL](test, L);
                REPORT(false);

        case 1: ON_LOOP(test->len) YIELD(2);
                test->put[SHINE](test, L);
                REPORT(false);

        case 2: ON_LOOP(30) YIELD(3);
                REPORT(false);

        case 3: ON_LOOP(test->len) YIELD(4);
                test->put[PUSH_R](test, L);
                REPORT(false);

        case 4: ON_LOOP(10) UNLOOP;
                REPORT(false);

        }
        doupdate();

        return (STATUS);
} 



#define RED_SHINY PUR_BRZ, PUR_RED
#define PIN_SHINY PUR_PIN, PUR_RED
#define EEN_SHINY PUR_SKY, PUR_EEN
#define BLU_SHINY PUR_SKY, PUR_BLU
#define GRE_SHINY PUR_GRE, PUR_SKY
#define YEL_SHINY PUR_BRZ, PUR_YEL
#define BEI_SHINY PUR_BEI, PUR_BRZ
#define WOO_SHINY PUR_BEI, PUR_WOO

enum styles { SWEEP, OVERSWEEP, UNSWEEP, COLLAPSE_R, COLLAPSE_L };


struct cgloss_t {const char *msg; int len; short co; short hi;};
static struct cgloss_t msg[]={ 
        {"Cancelled!     " , 15, RED_SHINY } ,
        {"Smack you!     " , 15, YEL_SHINY } ,
        {"Porkchop!      " , 15, BEI_SHINY } ,
        {"Beast man!     " , 15, EEN_SHINY } ,
        {"Combat!        " , 15, PUR_GRE, PUR_GRE} ,
        {"A gentle touch!" , 15, PUR_GRE, PUR_GRE} ,
        {"Tasty!         " , 15, BEI_SHINY } ,
        {"Eureka!        " , 15, BLU_SHINY } ,
        {"Careful!       " , 15, RED_SHINY } ,
        {"My eye!        " , 15, PIN_SHINY } ,
        {"Jemmy legs!    " , 15, GRE_SHINY } ,
        {"With a rope!   " , 15, WOO_SHINY } ,
        {"Have at you!   " , 15, GRE_SHINY } ,
        {"Santiago!      " , 15, YEL_SHINY } ,
        {"Belches!       " , 15, EEN_SHINY } ,
        {"My lewt!       " , 15, PIN_SHINY } ,
        {"Sticky buns!   " , 15, YEL_SHINY } ,
        {"Sink you!      " , 15, BLU_SHINY }
};
