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
        #define     UNLOOP      STAGE=LOOP=0; return false

        #define LOOPVAR(v) v = (LOOP)

        static unsigned int LOOP;
        static unsigned int STAGE;
        static unsigned int SKIP;
        static bool STATUS;
        int L;

        LOOPVAR(L);

        switch (STAGE)
        {
        case 0: ON_LOOP(test->len) YIELD(1);
                test->put[REVEAL](test, L);
                REPORT(true);

        case 1: ON_LOOP(test->len) YIELD(2);
                test->put[SHINE](test, L);
                REPORT(true);

        case 2: ON_LOOP(30) YIELD(3);
                REPORT(true);

        case 3: ON_LOOP(test->len) YIELD(4);
                test->put[PUSH_R](test, L);
                REPORT(true);

        case 4: ON_LOOP(10) UNLOOP;
                REPORT(true);

        }
        doupdate();

        return (STATUS);
} 




