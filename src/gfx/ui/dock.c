#include "../../com/arawak.h"
#include "../../lib/textutils.h"
#include "../../noun/noun.h"
#include "../../txt/gloss.h"
#include "dock.h"


/* DOCK CONSTANTS AND STATIC VARIABLES 
``````````````````````````````````````````````````````````````````````````````*/
/* Widths for the subdivisions of the dock buffer */
#define item_field_w (20)
#define stat_field_w (16)
#define text_field_w (COLS - stat_field_w - item_field_w - 3)

/* Dock buffer dimensions */
#define dock_h (1)
#define dock_w (COLS)
#define dock_y (LINES-dock_h)
#define dock_x (0)

/* Gloss buffer dimensions */
#define gloss_h (dock_h)
#define gloss_w (text_field_w/2)
#define gloss_y (dock_y)
#define gloss_x (item_field_w)


/* Windows and panels */
WINDOW *dock_win;
PANEL  *dock_pan;
WINDOW *gloss_win;
PANEL  *gloss_pan;

/* Values used to print the dock and gloss buffer */
char    *EQUIPNAME;  /* Name of the currently equipped item */
wchar_t *EQUIPICON;  /* Icon of the currently equipped item */
wchar_t *SPEAKER;    /* Icon of the current speaker */
char    *MESSAGE;    /* Text of the current speaker's message */
GLOSS   *GLOSSMSG;   /* Gloss object if notification active */
STATS   STAT_WORD;   /* Character stat word of the player character */



/* DOCK VARIABLES 
``````````````````````````````````````````````````````````````````````````````*/
/* EQUIPMENT
 * say_equip -- specify an icon and name for the currently equipped item
 * @icon: wide-character icon
 * @name: character string
 */
void say_equip(wchar_t *icon, char *name)
{
        release((void **)&EQUIPNAME);
        release((void **)&EQUIPICON);
        EQUIPNAME = mydup(name); 
        EQUIPICON = wcsdup(icon); 
}


/* SPEAK 
 * say -- specify an icon for a speaker and the message they should say 
 * @icon: wide-character icon depicting a speaker's portrait
 * @msg : message text (what they say)
 */
void say_speak(wchar_t *speaker, char *message)
{
        release((void **)&SPEAKER);
        release((void **)&MESSAGE);
        SPEAKER = wcsdup(speaker); 
        MESSAGE = mydup(message); 
}


/* STATS
 * say_stats -- specify character statistics to decompose and print
 * @stat: STATS word
 */
void say_stats(STATS stats)
{
        STAT_WORD = stats;
}


/* GLOSS 
 * say_gloss -- create a gloss object that will be printed in the buffer 
 * @msg: textual content to be glossed
 * @hi : highlight color of the gloss
 * @lo : standard  color of the gloss
 */
void say_alert(wchar_t *msg, short hi, short lo)
{
        release((void **)&GLOSSMSG);
        GLOSSMSG = new_gloss(gloss_win, msg, hi, lo);        
}



/* DOCK INITIALIZATION
``````````````````````````````````````````````````````````````````````````````*/
/**
 * init_dock -- create the dock and gloss structures for the first time
 */
void init_dock(void)
{
        if (dock_pan) 
                return; 

        say_equip(L"","");
        say_speak(L"","");
        say_stats(0x00000008UL);

        dock_win = newwin(dock_h, dock_w, dock_y, dock_x);
        dock_pan = new_panel(dock_win);

        gloss_win = newwin(gloss_h, gloss_w, gloss_y, gloss_x);
        gloss_pan = new_panel(gloss_win);

        wbkgrnd(gloss_win, &PURPLE[2]);
        wbkgrnd(dock_win,  &PURPLE[2]);
}



/* DOCK VISIBILITY 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * view_dock -- make the dock panels visible at the bottom of the screen
 */
void view_dock(void)
{
        show_panel(dock_pan);
        scr_refresh();
}

/**
 * hide_dock -- hide the dock panels so they are no longer visible
 */
void hide_dock(void)
{
        hide_panel(dock_pan);
        scr_refresh();
}

/**
 * dock_toggle -- toggle the dock view between hidden and visible
 */
void dock_toggle(void) 
{
        (panel_hidden(dock_pan)) ? view_dock() : hide_dock();
}



/* DOCK PRINTING AND OPERATIONS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * do_gloss -- step the gloss rendering forward by one tick
 */
inline void do_gloss(void)
{
        static bool running = false;

        if (GLOSSMSG) {
                show_panel(gloss_pan);
                running = gloss(GLOSSMSG);
                if (!running) {
                        del_gloss(GLOSSMSG);
                        release((void **)&GLOSSMSG);
                        hide_panel(gloss_pan);
                }
        }
}

/**
 * print_dock -- print the dock buffer
 */
void print_dock(void)
{
        #define STAT_SPLIT(s) HP(s), SP(s), AP(s)
        #define SZ 300
        // ༈∰∯∮◆◈◇ℌℜ

        wchar_t item_field[SZ];
        wchar_t text_field[SZ];
        wchar_t stat_field[SZ];

        swpumpf(item_field, SZ, L"  %ls %s", EQUIPICON, EQUIPNAME);
        swpumpf(text_field, SZ, L"%ls %s",   SPEAKER, MESSAGE);
        swpumpf(stat_field, SZ, L"༈ ⦗%02u⦘⦗%02u⦘⦗%02u⦘", STAT_SPLIT(STAT_WORD));

        if (!dock_pan)
                init_dock();

        do_gloss(); /* Draw the gloss message if it exists */
        werase(dock_win);

        wpumpw(dock_win, L"%-*ls %-*ls %*ls", item_field_w, item_field,
                                              text_field_w, text_field,
                                              stat_field_w, stat_field);

        /*mvwchgat(dock_win, 0, 0, item_field_w, 0, item_pair, NULL);*/
}

