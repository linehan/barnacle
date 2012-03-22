#define _XOPEN_SOURCE_EXTENDED = 1
#include "../gfx/stdpan.h"
#include "../gfx/dock.h"

inline void type(WINDOW *win, const char *str, int len)
{
        int i;
        
        if (len == strlen(str)) return;

        werase(win);
        wrefresh(win);
        for (i=0; i<len; i++) {
                wprintw(win, "%c", str[i]);
        }
        wrefresh(win);
}


inline void untype(WINDOW *win, const char *str, int l)
{
        int i;
        int len;
        
        len = (strlen(str)-l);

        if (len == 0) return;

        werase(win);
        wrefresh(win);
        for (i=0; i<len; i++) {
                wprintw(win, "%c", str[i]);
        }
        wrefresh(win);
}


inline void collapse(WINDOW *win, const char *str, int l)
{
        int len;
        int i;
        
        len = (strlen(str));

        werase(win);
        wrefresh(win);
        for (i=l; i<len; i++) {
                wprintw(win, "%c", str[i]);
        }
        wrefresh(win);
}
inline bool fadeout(WINDOW *win, const char *str)
{
        #define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
        static uint32_t mask = 0x00000001;
        static uint32_t card;
        static int len;
        static bool ready;
        int i, tmp;
        char c;

        if (ready == false) {
                len = strlen(str);
                card = (~0<<len);
                ready = true;
        }

        if (card == ~(0)) ready = false;

        tmp = ones(card);

        while (tmp == ones(card) && ready == true)
                card |= (mask<<(roll_fair(len)));

        werase(win);
        wrefresh(win);
        for (i=0; i<10; i++) {
                c = (CHECK_BIT(card, i)!=0) ? ' ' : str[i];
                wprintw(win, "%c", c);
        }
        wrefresh(win);

        return (ready);
}



enum styles { SWEEP, OVERSWEEP, UNSWEEP, COLLAPSE_R, COLLAPSE_L };

#define RED_SHINY PUR_BRZ, PUR_RED
#define PIN_SHINY PUR_PIN, PUR_RED
#define EEN_SHINY PUR_SKY, PUR_EEN
#define BLU_SHINY PUR_SKY, PUR_BLU
#define GRE_SHINY PUR_GRE, PUR_SKY
#define YEL_SHINY PUR_BRZ, PUR_YEL
#define BEI_SHINY PUR_BEI, PUR_BRZ
#define WOO_SHINY PUR_BEI, PUR_WOO

struct msg_t { const char *msg; int len; short color; short hi; };
static struct msg_t msg[]={ 
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


inline void fpost(WINDOW *win, const char *str, int l, int style, short co)
{
        int len = strlen(str);
        int i;
        
        #define NSTYLES 5
        int pad[NSTYLES]={0 , 0 , l     , l       , 0  };
        int ini[NSTYLES]={0 , 0 , (l+1) , 0       , l  };
        int fin[NSTYLES]={l , 0 , len   , (len-l) , len};

        wcolor_set(win, co, NULL);

        switch (style)
        {
        case OVERSWEEP:
                mvwprintw(win, 0, l-1, "%c", str[l-1]);
                wrefresh(win);
                doupdate();
                return;

        case COLLAPSE_R:
        case COLLAPSE_L:
        case SWEEP:
        case UNSWEEP:
                werase(win);
                wrefresh(win);
                wmove(win, 0, pad[style]);

                for (i=ini[style]; i<fin[style]; i++) {
                        wprintw(win, "%c", str[i]);
                        wrefresh(win);
                }
                doupdate();
                break;
        }
}

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
bool say(int opt)
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

        WINDOW *win = dock_window(SUBJ_TX_WIN);
        
        LOOPVAR(L);

        switch (STAGE)
        {
        case 0: ON_LOOP((msg[opt].len)) YIELD(1);

                fpost(win, msg[opt].msg, L, SWEEP, msg[opt].color);
                if (L>5) fpost(win, msg[opt].msg, L-5, OVERSWEEP, msg[opt].hi);

                REPORT(false);

        case 1: ON_LOOP((msg[opt].len)) YIELD(2);

                fpost(win, msg[opt].msg, L, OVERSWEEP, msg[opt].hi);

                REPORT(false);

        case 2: ON_LOOP(30) YIELD(3);

                REPORT(false);

        case 3: ON_LOOP((msg[opt].len)) YIELD(4);

                fpost(win, msg[opt].msg, L, COLLAPSE_R, msg[opt].hi);

                REPORT(false);

        case 4: ON_LOOP(10) UNLOOP;

                REPORT(false);
        }
        doupdate();

        return (STATUS);
} 

