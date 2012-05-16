#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>
#include <stdbool.h>


#define DEC(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define INC(x, max) x = ((x) < (max)) ? ((x)+1) : (x)


#define SWATCH_SIZE 2
#define NUM_SWATCHES COLS/SWATCH_SIZE

#define COLOR_HEIGHT 2
#define PAIR_HEIGHT  2
#define MAIN_HEIGHT (LINES-COLOR_HEIGHT-PAIR_HEIGHT)
#define MAIN_WIDTH  (COLS)

#define CURSOR_COLOR WHITE

WINDOW *main_win;
WINDOW *color_win;
WINDOW *pair_win;
WINDOW *cursor_win;
PANEL  *main_pan;
PANEL  *color_pan;
PANEL  *pair_pan;
PANEL  *cursor_pan;


short color[50];
short pair[50];

enum basic_colors {
        _WHITE,
        _BLACK,
        DGREY,
        GREY
};
enum basic_pairs {
        MONO,
        BASIC,
        CONTRAST,
        CONTRAST
};

inline void ncurses_start(void)
{
        initscr();
        start_color();         /* Enable terminal colors */
        cbreak();	       /* Do not wait for newline (line buffering) */
        noecho();              /* Do not echo input to screen */
        curs_set(0);           /* Do not display cursor */
        nodelay(stdscr, true); /* Calls to getch() are non-blocking */ 
        keypad(stdscr, true);  /* Enable special keys */
}


cchar_t shared_cch;

inline cchar_t *mkcch(wchar_t *wch, attr_t attr, short co)
{
        setcchar(&shared_cch, wch, attr, co, NULL);
        return (&shared_cch);
}

inline void init_basic_palette(void)
{
        init_color(_WHITE, 1000, 1000, 1000);
        init_color(_BLACK, 0, 0, 0);
        init_color(DGREY, 78,   71,   78);
        init_color(GREY,  564,  564,  564);
        init_pair(MONO, _BLACK, _WHITE);
        init_pair(BASIC, _WHITE, _BLACK);
        init_pair(CONTRAST1, _BLACK, GREY);
        init_pair(CONTRAST2, _BLACK, DGREY);
}


inline void init_editor(void)
{
        ncurses_start();
        init_basic_palette();

        main_win  = newwin(LINES, COLS, 0, 0);
        color_win = newwin(COLOR_HEIGHT, COLS, LINES-COLOR_HEIGHT-PAIR_HEIGHT, 0);
        pair_win  = newwin(PAIR_HEIGHT, COLS, LINES-PAIR_HEIGHT, 0);
        cursor_win = newwin(1, 1, 0, 0);

        main_pan  = new_panel(main_win);
        color_pan = new_panel(color_win);
        pair_pan  = new_panel(pair_win);
        cursor_pan = new_panel(cursor_win);

        wbkgrnd(cursor_win, mkcch(L"█", 0, MONO));
        wbkgrnd(color_win,  mkcch(L"█", 0, CONTRAST));
        wbkgrnd(pair_win,   mkcch(L"█", 0, CONTRAST));
        wbkgrnd(main_win,   mkcch(L" ", 0, BASIC));

        update_panels();
        doupdate();
}


inline void move_cursor(int dir, int step)
{
        static int cur_y;
        static int cur_x;

        switch (dir) {
        case 'l':
                while (step-->0) DEC(cur_x,0);
                break;
        case 'r':
                while (step-->0) INC(cur_x,MAIN_WIDTH);
                break;
        case 'u':
                while (step-->0) DEC(cur_y,0);
                break;
        case 'd':
                while (step-->0) INC(cur_y,MAIN_HEIGHT);
                break;
        }

        move_panel(cursor_pan, cur_y, cur_x);
        top_panel(cursor_pan);
        update_panels();
}


int main(void)
{
        int c;

        if (!main_win)
                init_editor();

        while (c = getch(), c != 'q') 
        {
                switch (c)
                {
                case 'h':
                        move_cursor('l', 1);
                        break;
                case 'H':
                        move_cursor('l', 4);
                        break;
                case 'l':
                        move_cursor('r', 1);
                        break;
                case 'L':
                        move_cursor('r', 4);
                        break;
                case 'k':
                        move_cursor('u', 1);
                        break;
                case 'K':
                        move_cursor('u', 4);
                        break;
                case 'j':
                        move_cursor('d', 1);
                        break;
                case 'J':
                        move_cursor('d', 4);
                        break;
                case 'g':

                }
                update_panels();
                doupdate();
        }

        endwin();
        return 0;
}

