#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <panel.h>

#include "button.h"
#include "../gfx/gfx.h"

struct button_t *new_button(const char *message, short norm, short hi, short grey)
{
        struct button_t *new = malloc(sizeof(struct button_t));

        memcpy(new->message, message, (size_t)strlen(message)+1);
        new->pair[0] = norm;
        new->pair[1] = hi;
        new->pair[2] = grey;
        new->win     = NULL;

        return (new);
}

void post_button(struct button_t *button)
{
        wattr_on(button->win, COLOR_PAIR(button->pair[0]), NULL);
        wprintw(button->win, "%s", button->message);
        win_refresh(button->win);
}

void focus_button(struct button_t *button)
{
        werase(button->win);
        wattr_set(button->win, A_REVERSE, button->pair[1], NULL);
        wprintw(button->win, "%s", button->message);
        win_refresh(button->win);
}

void unfocus_button(struct button_t *button)
{
        werase(button->win);
        wattr_set(button->win, A_NORMAL, button->pair[0], NULL);
        wprintw(button->win, "%s", button->message);
        win_refresh(button->win);
}

void put_button(struct button_t *button, WINDOW *win, int h, int w, int y, int x)
{
        button->win = derwin(win, h, w, y, x);
        post_button(button);
        win_refresh(button->win);
}
