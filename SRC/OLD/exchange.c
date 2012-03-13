#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <stdint.h>

#include "../gen/dice.h"
#include "../guy/model/guy.h"


struct acp_t {
        int amplitude;
        int threshold;
};

struct acp_t reg_acptest[4]={{1,2},{3,10},{5,4},{0,64}};

/*
  Returns the combined width (offset) of the 4 vitals for the creature
  indexed by 'key'.
*/
inline int acp_offset(uint32_t key)
{
        int i, v=0;
        for (i=0; i<NBYTES; i++) {
                v += get_vital(&key, i);
        }
        return (v);
}

/*
  Returns 1 if the request for action 'action_id' succeeds, else returns 0.
*/
int acp_request(int action_id, uint32_t key)
{
        int a, t;

        a = reg_acptest[action_id].amplitude;
        t = reg_acptest[action_id].threshold;
        t -= acp_offset(key);

        return (a >= t) ? 1 : 0;
}

void acp_test(void)
{
        #include "../pan/test.h"

        int i;
        for (i=0; i<4; i++) {
                wprintw(DIAGNOSTIC_WIN, "key: %u\tacp: %i\tresult: %i\n", 
                        muster[i], i, acp_request(i, muster[i]));
        }
}

