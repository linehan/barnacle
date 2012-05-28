#include <assert.h>
#include "../../com/arawak.h"
#include "../../txt/gloss.h"
#include "../../lib/textutils.h"
#include "../../lib/stoc/stoc.h"
#include "dock.h"
#include "notify.h"
#include "../../test/test.h"


void picksay(const wchar_t *icon, struct phrase_t *phrase)
{
        int choice;
        choice = (roll_fair(phrase->n)%phrase->n);
        say_speak(icon, phrase->say[choice]);
}


/*
 * alert -- create a gloss object to do the talking 
 * @msg: the message to be used in the gloss object
 */
void alert(enum alerts tag, char *msg)
{
        wchar_t wcs[200];

        wclean(wcs, 200);

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


