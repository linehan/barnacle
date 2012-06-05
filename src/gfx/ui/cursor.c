/* 
 * cursor.c -- cursor object and its methods
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
#include "cursor.h"


/*
 * Forward declarations for cursor methods
 */
void method_cursor_hide(void *self);
void method_cursor_show(void *self);
void method_cursor_stamp(void *self, WINDOW *win);
void method_cursor_move(void *self, int input);



/**
 * new_cursor -- create a new cursor object
 * @h_cur: height of the cursor
 * @w_cur: width of the cursor
 * @y_cur: starting y-coordinate of the cursor
 * @x_cur: starting x-coordinate of the cursor
 * @h_scr: height of the screen the cursor can move in
 * @w_scr: width of the screen the cursor can move in
 * @y_scr: starting y-coordinate of the screen
 * @x_scr: starting x-coordinate of the screen
 * @keys : the keycluster that the cursor should respond to
 */
struct cursor_t *new_cursor(int h_cur, int w_cur, int y_cur, int x_cur,
                            int h_scr, int w_scr, int y_scr, int x_scr,
                            enum keycluster keys)
{
        #define DEFAULT_CURSOR_STEP 4
        struct cursor_t *new = calloc(1, sizeof(struct cursor_t));

        new->pos = new_pos(h_cur, w_cur, y_cur, x_cur, 
                           h_scr, w_scr, y_scr, x_scr);

        new->win = newwin(h_cur, w_cur, y_cur, x_cur);
        new->pan = new_panel(new->win);

        new->keys = keys;
        new->step = DEFAULT_CURSOR_STEP;

        new->Show  = &method_cursor_show;
        new->Hide  = &method_cursor_hide;
        new->Stamp = &method_cursor_stamp;
        new->Move  = &method_cursor_move;

        return (new);
}


/******************************************************************************
 * Cursor methods
 *
 * method_cursor_hide
 * method_cursor_show
 * method_cursow_move
 *
 ******************************************************************************/

/**
 * method_cursor_hide -- hide the cursor PANEL
 * @self: self object
 */
void method_cursor_hide(void *self)
{
        struct cursor_t *cursor = (struct cursor_t *)self;
        hide_panel(cursor->pan);
}


/**
 * method_cursor_show -- show the cursor PANEL
 * @self: self object
 */
void method_cursor_show(void *self)
{
        struct cursor_t *cursor = (struct cursor_t *)self;
        show_panel(cursor->pan);
}


/**
 * method_cursor_stamp -- copy the cursor into the desired WINDOW below 
 * @self: self object
 * @win : WINDOW to which the cursor will be copied (at pos_y, pos_x)
 */
void method_cursor_stamp(void *self, WINDOW *win)
{
        struct cursor_t *cursor = (struct cursor_t *)self;

        copywin(cursor->win, win,
                0, 0,                                           /* srcmin   */ 
                pos_y(cursor->pos), pos_x(cursor->pos),         /* dstmin   */ 
                pos_y(cursor->pos) + pos_h(cursor->pos) - 1,    /* dstmax y */
                pos_x(cursor->pos) + pos_w(cursor->pos) - 1,    /* dstmax x */
                0);                                             /* overlay? */
}


/**
 * method_cursor_move -- move the cursor PANEL in response to some input 
 * @self : self object
 * @input: a keypress
 */
void method_cursor_move(void *self, int input)
{
        struct cursor_t *cursor = (struct cursor_t *)self;

        switch (cursor->keys) 
        {
        case HJKL:
                switch (input) 
                {
                case 'h':
                        pos_l(cursor->pos);
                        break;
                case 'j':
                        pos_d(cursor->pos);
                        break;
                case 'k':
                        pos_u(cursor->pos);
                        break;
                case 'l':
                        pos_r(cursor->pos);
                        break;
                case 'H':
                        pos_lstep(cursor->pos, cursor->step);
                        break;
                case 'J':
                        pos_dstep(cursor->pos, cursor->step);
                        break;
                case 'K':
                        pos_ustep(cursor->pos, cursor->step);
                        break;
                case 'L':
                        pos_rstep(cursor->pos, cursor->step);
                        break;
                }
        case WASD:
                switch (input) 
                {
                case 'w':
                        pos_u(cursor->pos);
                        break;
                case 'a':
                        pos_l(cursor->pos);
                        break;
                case 's':
                        pos_d(cursor->pos);
                        break;
                case 'd':
                        pos_r(cursor->pos);
                        break;
                case 'W':
                        pos_ustep(cursor->pos, cursor->step);
                        break;
                case 'A':
                        pos_lstep(cursor->pos, cursor->step);
                        break;
                case 'S':
                        pos_dstep(cursor->pos, cursor->step);
                        break;
                case 'D':
                        pos_rstep(cursor->pos, cursor->step);
                        break;
                }
        }
        move_panel(cursor->pan, pos_y(cursor->pos), pos_x(cursor->pos));
}



