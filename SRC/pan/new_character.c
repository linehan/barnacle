// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <form.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../gfx/wselect.h"
#include "../gen/name/name.h"
#include "ctrlpanels.h"
#include "../guy/guy.h"
#include "../guy/jobs.h"
#include "../gfx/fancy_menus.h"
#include "button.h"

#define KEY_ESC 27 // Char value of ESC
#define W_FIELD 20
#define H_FIELD 1

#define NFIELDS 5

#define MAX_PEOPLE 100
uint32_t person[MAX_PEOPLE];


char *strstrip(char *s)
{
        size_t size;
        char *cpy, *end;

        /* Make a copy of the string at *s */
        size = strlen(s);
        cpy = calloc(size, sizeof(char));
        memcpy(cpy, s, size);

        if (!size)
                return (cpy);

        end = cpy + size - 1;
        while (end >= cpy && isspace(*end))
        end--;
        *(end + 1) = '\0';

        while (*cpy && isspace(*cpy))
        cpy++;

        return (cpy);
}



void new_character(void)
{
        FIELD   *field[5];
        FORM    *sheet;
        WSELECT *sel;
        struct fancy_menu *jobs;
        struct button_t *ok_button, *cancel_button;

        char *firname, *midname, *lasname, *birthpl;
        uint32_t key;
        int ch, i;


        /* ncurses sets the ESC key delay at 100ms by default, and this
         * is way too slow. */
        if (getenv ("ESCDELAY") == NULL) ESCDELAY = 25;


        /* Select a character portrait. */
        sel = new_wselect(L"餮鸎愚寫亶䴎戀念鑍嶏愈", 11, PUR_PUR, PUR_DYE, PUR_GRE, PUR_YEL);
        wselect_set_win(sel, INFOW[BODY]);
        wselect_set_ofs(sel, 0, 15);
        wselect_paint(sel);


        /* Enter names and other strings. */
        field[0] = new_field(H_FIELD, W_FIELD, 2, 15, 0, 0);
        field[1] = new_field(H_FIELD, W_FIELD, 4, 15, 0, 0);
        field[2] = new_field(H_FIELD, W_FIELD, 6, 15, 0, 0);
        field[3] = new_field(H_FIELD, W_FIELD, 8, 15, 0, 0);
        field[4] = NULL;
        for (i=0; i<4; i++) {
                field_opts_off(field[i], O_AUTOSKIP);
        }
        sheet = new_form(field);
        set_form_win(sheet, INFOW[BORD]);
        set_form_sub(sheet, INFOW[BODY]);
        post_form(sheet);


        /* Select a profession from a list with descriptions. */
        jobs = new_fancy_menu(job_list, job_flavor, PUR_GRE, PUR_PUR);
        put_fancy_menu(jobs, INFOW[BODY], 1, 45, 10, 15);
        set_fancy_detail(jobs, 2, 40, 11, 15, PUR_PUR);
        set_fancy_select(jobs, PUR_YEL);
        post_fancy_menu(jobs);
        post_fancy_detail(jobs);


        /* Make some buttons to say OK and CANCEL. */
        ok_button = new_button("OK", 0, PUR_YEL, PUR_PUR);
        put_button(ok_button, INFOW[BODY], 1, 5, 15, 0);

        cancel_button = new_button("CANCEL", 0, PUR_YEL, PUR_PUR);
        put_button(cancel_button, INFOW[BODY], 1, 7, 15, 5);

        /* Print the text that goes with these things. */
        mvwprintw(INFOW[HEAD], 0, 0, "Create a new character"); /* Title */
        mvwprintw(INFOW[BODY], 0, 0, "Portrait");
        mvwprintw(INFOW[BODY], 2, 0, "Given name");
        mvwprintw(INFOW[BODY], 4, 0, "Middle name");
        mvwprintw(INFOW[BODY], 6, 0, "Surname");
        mvwprintw(INFOW[BODY], 8, 0, "Birthplace");
        mvwprintw(INFOW[BODY], 10, 0, "Profession");

        TOGGLE_INFO;
        vrt_refresh();

        /* Focus and select the portrait field, since it's at the top. */
        wselect_driver(sel, SEL_FOCUS);
        wselect_driver(sel, SEL_FIRST);

        i = 0;

	while ((ch = wgetch(INFOW[BODY])) != KEY_ESC) {

                if (ch == KEY_DOWN)     goto next_item;
                if (ch == KEY_UP)       goto prev_item;
                else                    goto get_input;
                
                rejoin_loop:

                scr_refresh();
        }
        goto clean_up;


        prev_item: /* Focus is transferred to the previous item on the menu. */

                switch (i) {
                case 0:
                        wselect_driver(sel, SEL_UNFOCUS);
                        focus_button(cancel_button);
                        break;
                case 1:
                        curs_set(0);
                        wselect_driver(sel, SEL_FOCUS);
                        break;
                case 2:
                case 3:
                case 4:
                        form_driver(sheet, REQ_PREV_FIELD);
                        break;
                case 5:
                        tog_fancy_select(jobs);
                        curs_set(1);
                        form_driver(sheet, REQ_LAST_FIELD);
                        break;
                case 6:
                        unfocus_button(ok_button);
                        tog_fancy_select(jobs);
                        break;
                case 7:
                        unfocus_button(cancel_button);
                        focus_button(ok_button);
                        break;
                }

                if (i == 0) i = 7;
                else        i--;

                goto rejoin_loop;


        next_item: /* Focus is transferred to the next item on the menu. */

                switch (i) {
                case 0:
                        wselect_driver(sel, SEL_UNFOCUS); 
                        form_driver(sheet, REQ_FIRST_FIELD);
                        curs_set(1);
                        break;
                case 1:
                case 2:
                case 3:
                        form_driver(sheet, REQ_NEXT_FIELD);
                        break;
                case 4:
                        curs_set(0);
                        tog_fancy_select(jobs);
                        break;
                case 5:
                        tog_fancy_select(jobs);
                        focus_button(ok_button);
                        break;
                case 6:
                        unfocus_button(ok_button);
                        focus_button(cancel_button);
                        break;
                case 7:
                        unfocus_button(cancel_button);
                        wselect_driver(sel, SEL_FOCUS);
                        break;
                }

                if (i == 7) i = 0;
                else        i++;

                goto rejoin_loop;


        get_input: /* The user's input manipulates the current item. */

                switch (i) {
                case 0:
                        switch (ch) {
                        case KEY_LEFT:
                                wselect_driver(sel, SEL_PREV);
                                break;
                        case KEY_RIGHT: 
                                wselect_driver(sel, SEL_NEXT);
                                break;
                        case ' ':
                                wselect_driver(sel, SEL_SELECT);
                                break;
                        }
                        break;
                case 1:
                case 2:
                case 3:
                case 4:
                        switch (ch) {
                        case KEY_BACKSPACE:
                                form_driver(sheet, REQ_DEL_PREV);
                                break;
                        default:
                                form_driver(sheet, ch);
                                break;
                        }
                        break;
                case 5:
                        switch(ch) {
                        case 'j':
                                menu_driver(jobs->menu, REQ_DOWN_ITEM);
                                post_fancy_detail(jobs);
                                break;
                        case 'k':
                                menu_driver(jobs->menu, REQ_UP_ITEM);
                                post_fancy_detail(jobs);
                                break;
                        }
                        break;
                case 6:
                        if (ch == '\n') {
                                unfocus_button(ok_button);
                                goto ok_pressed;
                        }
                        break;
                case 7:
                        if (ch == '\n') {
                                unfocus_button(cancel_button);
                                goto clean_up;
                        }
                        break;
                }
                goto rejoin_loop;


        ok_pressed: /* The user has decided to confirm and end the menu. */

                firname = strstrip(field_buffer(field[0], 0));
                midname = strstrip(field_buffer(field[1], 0));
                lasname = strstrip(field_buffer(field[2], 0));
                birthpl = strstrip(field_buffer(field[3], 0));

                /* Register a new player character. */
                new_guy(firname, midname, lasname, birthpl, 0, 0, 0, 0, 0);

                wprintw(DIAGNOSTIC_WIN, "key: %u\n", npersons());
        
        clean_up:

                curs_set(0);
                TOGGLE_INFO;
                vrt_refresh();
                scr_refresh();

                unpost_form(sheet);
                free_form(sheet);
                free_field(field[0]);
                free_field(field[1]);
                free_field(field[2]);
                free_field(field[3]);
}
