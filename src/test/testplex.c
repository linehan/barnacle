#include "testplex.h"
#include "tests/allheader.h"

bool testplex_online = false;

void init_testplex(void)
{
        int i;

        TESTPAL = new_pal();
        pal_load(TESTPAL, TESTPAL_PATH"/"TESTPAL_NAME, FIRST_TESTPAL_PAIRNUM);

        for (i=0; i<NUM_TEST_WINDOWS; i++) {
                TESTWIN[i] = newwin(LINES, COLS, 0, 0);
                TESTPAN[i] = new_panel(TESTWIN[i]);
        }
}




void testplex(int testpanel_index)
{
        if (!testplex_online) init_testplex();
        top_panel(TESTPAN[testpanel_index]);


        pmap_test(testpanel_index, LINES-10, COLS-10);
}
