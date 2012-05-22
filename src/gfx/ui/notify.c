#include <assert.h>
#include "../../com/arawak.h"
#include "../../txt/gloss.h"
#include "../../lib/textutils.h"
#include "dock.h"
#include "notify.h"
#include "../../test/test.h"


/*
 * alert -- create a gloss object to do the talking 
 * @msg: the message to be used in the gloss object
 */
void alert(enum alerts tag, char *msg)
{
        wchar_t *wcs;
        int scheme;

        switch (tag) {
        case I_FIND:
                wpumpf(&wcs, L"Found %s", msg);
                say_alert(wcs, __GOLD);
                break;
        case I_KILL:
                wpumpf(&wcs, L"Killed %s", msg);
                say_alert(wcs, __KILL);
                break;
        }

        free(wcs);
}


