#include <assert.h>
#include "../../com/arawak.h"
#include "../../txt/gloss.h"
#include "../../lib/textutils.h"
#include "dock.h"
#include "notify.h"
#include "../../test/test.h"

#define DOCK dock_win(TEXT_WIN)

/*
 * say -- print a message to the dock messager
 * @who: the portrait of the "person" doing the talking
 * @msg: the string message
 */
void say(const wchar_t *who, const char *msg)
{
        werase(DOCK);

        mvwadd_wch(DOCK, 0, 0, mkcch(who, 0, PUR_YELLOW));
        wcolor_set(DOCK, PUR_YELLOW, NULL);
        mvwprintw(DOCK, 0, 3, "%s\n", msg);

        wrefresh(DOCK);
}


struct gloss_t *speak;


void do_gloss(void)
{
        static bool running = false;

        if (speak) {
                running = gloss(speak);
                if (!running) {
                        del_gloss(speak);
                        release(&speak);
                }
        }
}



/*
 * alert -- create a gloss object to do the talking 
 * @msg: the message to be used in the gloss object
 */
void alert(enum alerts tag, char *msg)
{
        char *string;
        int scheme;

        werase(DOCK);

        switch (tag) {
        case I_FIND:
                pumpf(&string, "Found %s", msg);
                speak = str_gloss(DOCK, string, __GOLD);
                break;
        case I_KILL:
                pumpf(&string, "Killed %s", msg);
                speak = str_gloss(DOCK, string, __KILL);
                break;
        }

        free(string);
}


