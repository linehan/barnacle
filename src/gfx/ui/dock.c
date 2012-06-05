/* 
 * dock.c -- Dock (information buffer) drawing and status.
 * The "dock" is a small buffer drawn at the bottom of the window, 
 * containing information and notifications for the player.
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "../../com/barnacle.h"
#include "../../eng/tick.h"
#include "../../lib/textutils.h"
#include "../../noun/noun.h"
#include "../../txt/gloss.h"
#include "dock.h"
#include "../../eng/loop.h"
#include "menu_inventory.h"


/* DOCK CONSTANTS AND STATIC VARIABLES 
``````````````````````````````````````````````````````````````````````````````*/
/* Widths for the subdivisions of the dock buffer */
#define item_field_w (16)
#define stat_field_w (17)
#define text_field_w (COLS - stat_field_w - item_field_w - 2)

/* X-offsets of the subdivisions of the dock buffer */
#define item_field_ofs (0)
#define stat_field_ofs (COLS - stat_field_w)
#define text_field_ofs (item_field_w)
#define stat_ofs(i)    (stat_field_ofs + 4 + ((i)*4))

/* Dock buffer dimensions */
#define dock_h (1)
#define dock_w (COLS)
#define dock_y (LINES-dock_h)
#define dock_x (0)

/* Gloss buffer dimensions */
#define gloss_h (dock_h)
#define gloss_w (text_field_w/2)
#define gloss_y (dock_y)
#define gloss_x (text_field_ofs + 2)


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
STATS   PREV_WORD;   /* The last set of character stats */


/* Times at which player stats or gloss messages were modified */
uint32_t STAT_TICK;
uint32_t GLOSS_TICK;


/* Whether each stat increased, decreased, or stayed the same */
enum stat_trend HP_DELTA;
enum stat_trend SP_DELTA;
enum stat_trend AP_DELTA;


/* DOCK ELEMENT ACCESSORS 
``````````````````````````````````````````````````````````````````````````````*/
/** 
 * say_equip -- specify an icon and name for the currently equipped item
 * @icon: wide-character icon
 * @name: character string
 */
void say_equip(wchar_t *icon, char *name)
{
        release((void **)&EQUIPNAME);
        release((void **)&EQUIPICON);
        EQUIPNAME = cdup(name); 
        EQUIPICON = wdup(icon); 
}


/**
 * say_speak -- specify an icon for a speaker and the message they should say 
 * @icon: wide-character icon depicting a speaker's portrait
 * @msg : message text (what they say)
 */
void say_speak(const wchar_t *speaker, const char *message)
{
        release((void **)&SPEAKER);
        release((void **)&MESSAGE);
        SPEAKER = wdup(speaker); 
        MESSAGE = cdup(message); 
}


/**
 * say_stats -- specify character statistics to decompose and print
 * @stat: STATS word
 */
void say_stats(STATS stats)
{
        HP_DELTA = stat_trend(HP(stats), HP(STAT_WORD));
        AP_DELTA = stat_trend(AP(stats), AP(STAT_WORD));
        SP_DELTA = stat_trend(SP(stats), SP(STAT_WORD));

        PREV_WORD = STAT_WORD;
        STAT_WORD = stats;
        STAT_TICK = get_tick(); 
}


/** 
 * say_alert -- create a gloss object that will be printed in the buffer 
 * @msg: textual content to be glossed
 * @hi : highlight color of the gloss
 * @lo : standard  color of the gloss
 */
void say_alert(wchar_t *msg, short hi, short lo)
{
        if (GLOSSMSG) {
                while (gloss(GLOSSMSG))  /* Unwind the gloss */
                ;
                del_gloss(GLOSSMSG);
                werase(gloss_win);
        }
        release((void **)&GLOSSMSG);

        struct gloss_t *new = new_gloss(gloss_win, msg, hi, lo); 
        GLOSSMSG = new;
        GLOSS_TICK = get_tick();
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
        AP_SET(&STAT_WORD, 13);
        SP_SET(&STAT_WORD, 3);
        HP_SET(&STAT_WORD, 25);

        gloss_win = newwin(gloss_h, gloss_w, gloss_y, gloss_x);
        gloss_pan = new_panel(gloss_win);

        dock_win = newwin(dock_h, dock_w, dock_y, dock_x);
        dock_pan = new_panel(dock_win);

        wbkgrnd(gloss_win, &PURPLE[2]);
        wbkgrnd(dock_win,  &PURPLE[2]);
}



/* DOCK VISIBILITY 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * view_dock -- make the dock panels visible at the bottom of the screen
 */
void show_dock(void)
{
        show_panel(dock_pan);
        show_panel(equipped_pan);
        scr_refresh();
}

/**
 * hide_dock -- hide the dock panels so they are no longer visible
 */
void hide_dock(void)
{
        hide_panel(dock_pan);
        hide_panel(equipped_pan);
        scr_refresh();
}

/**
 * dock_toggle -- toggle the dock view between hidden and visible
 */
void dock_toggle(void) 
{
        (panel_hidden(dock_pan)) ? show_dock() : hide_dock();
}

/**
 * dock_update -- make sure that the dock stays on top if it is enabled
 */
void dock_update(void)
{
        if (!panel_hidden(dock_pan)) {
                top_panel(dock_pan);
                top_panel(equipped_pan);
        }
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
 * stat_trend_timeout -- re-sets the value of a stat_trend if time has past
 * @trend  : pointer to a enum stat_trend 
 * @persist: the length of time before the stat is "stale"
 */
inline void stat_trend_timeout(enum stat_trend *trend, int persist)
{
        if (get_tick() >= STAT_TICK + persist) 
                *trend = STAT_SAME;
}

/**
 * interruptor -- returns a periodic boolean value
 */
inline bool interruptor(void)
{
        const  int stride = 2;
        static int i;

        if (i++ == stride)
                i = 0;

        return i ? false : true;
}


/**
 * print_paused -- replace the dock buffer with a "paused" message and ticker
 */
void print_paused(void)
{
        static int i;
        static int j;
        if (i++ == 2) {
                j++;
                i=0;
        }
        hide_panel(equipped_pan);
        werase(dock_win);
        mvwpumpw(dock_win, 0, (COLS/2)-4, L"%lc Paused", wspin(j));
        update_panels();
        doupdate();
}


/**
 * print_dock -- print the dock buffer
 */
void print_dock(void)
{
        #define STAT_SPLIT(s) HP(s), SP(s), AP(s)
        #define STAT_PERSIST 5
        #define SZ 300

        // ༈∰∯∮◆◈◇ℌℜ⸠⸡⫿∣

        short hp_color[]={PUR_PURPLE, _PUR_LRED, _PUR_LGREEN};
        short ap_color[]={PUR_PURPLE, _PUR_LRED, __PUR_LBLUE};
        short sp_color[]={PUR_PURPLE, _PUR_LRED, PUR_BRZ};

        wchar_t item_field[SZ];
        wchar_t text_field[SZ];
        wchar_t stat_field[SZ];
        wclean(item_field, SZ);
        wclean(text_field, SZ);
        wclean(stat_field, SZ);

        swpumpf(item_field, SZ, L"%ls %s", EQUIPICON, EQUIPNAME);
        swpumpf(text_field, SZ, L"%ls %s",   SPEAKER, MESSAGE);
        swpumpf(stat_field, SZ, L"⦗%02u⦘⦗%02u⦘⦗%02u⦘", STAT_SPLIT(PREV_WORD));

        if (!dock_pan) {
                init_dock();
        }

        do_gloss(); /* Draw the gloss message if it exists */

        werase(dock_win);

        mvwpumpw(dock_win, 0, item_field_ofs, L"%ls", item_field);
        mvwpumpw(dock_win, 0, text_field_ofs, L"▏ %ls", text_field);
        mvwpumpw(dock_win, 0, stat_field_ofs, L"▕  %ls", stat_field);

        mvwchgat(dock_win, 0, stat_ofs(0), 2, 0, hp_color[HP_DELTA], NULL);
        mvwchgat(dock_win, 0, stat_ofs(1), 2, 0, sp_color[SP_DELTA], NULL);
        mvwchgat(dock_win, 0, stat_ofs(2), 2, 0, ap_color[AP_DELTA], NULL);

        if (interruptor()) {
                HP_DELTA = stat_seek(&STAT_WORD, &PREV_WORD, HP);
                AP_DELTA = stat_seek(&STAT_WORD, &PREV_WORD, AP);
                SP_DELTA = stat_seek(&STAT_WORD, &PREV_WORD, SP);
        }

        if (GLOSSMSG) {
                mvwchgat(dock_win, 0, text_field_ofs, 1, 0, gloss_hi(GLOSSMSG), NULL);
        }

        update_panels();
}


