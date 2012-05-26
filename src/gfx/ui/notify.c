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
        wchar_t wcs[200];
        int scheme;

        switch (tag) {
        case I_FIND:
                swpumpf(wcs, 200, L"Found %s", msg);
                say_alert(wcs, __GOLD);
                break;
        case I_KILL:
                swpumpf(wcs, 200, L"Killed %s!", msg);
                say_alert(wcs, __KILL);
                break;
        }
}


