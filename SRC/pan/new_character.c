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

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../gfx/wselect.h"
#include "../gen/name/name.h"
#include "ctrlpanels.h"
#include "jobs.h"
#include "../gfx/fancy_menus.h"

#define KEY_ESC 27 // Char value of ESC
#define W_FIELD 20
#define H_FIELD 1

#define NFIELDS 5


void new_character(void)
{
        enum field_tags { NAME, AGE, GENDER };
        FIELD *field[5];
        FORM  *sheet;
        WSELECT *sel;
        struct fancy_menu *jobs;
        int ch, i;

        if (getenv ("ESCDELAY") == NULL) ESCDELAY = 25;

        /* Character portraits */
        sel = new_wselect(L"餮鸎愚寫亶䴎戀念鑍嶏愈", 11, PUR_PUR, PUR_DYE, PUR_GRE, PUR_YEL);
        wselect_set_win(sel, INFOW[BODY]);
        wselect_set_ofs(sel, 0, 15);
        wselect_paint(sel);

        field[0] = new_field(H_FIELD, W_FIELD, 2, 15, 0, 0);
        field[1] = new_field(H_FIELD, W_FIELD, 4, 15, 0, 0);
        field[2] = new_field(H_FIELD, W_FIELD, 6, 15, 0, 0);
        field[3] = new_field(H_FIELD, W_FIELD, 8, 15, 0, 0);
        field[4] = NULL;

        mvwprintw(INFOW[HEAD], 0, 0, "Create a new character");

        /* Set field options */
        field_opts_off(field[0], O_AUTOSKIP); 
        field_opts_off(field[1], O_AUTOSKIP);
        field_opts_off(field[2], O_AUTOSKIP);
        field_opts_off(field[3], O_AUTOSKIP);

        /* Create form and post it */
        sheet = new_form(field);
        set_form_win(sheet, INFOW[BORD]);
        set_form_sub(sheet, INFOW[BODY]);
        post_form(sheet);
        TOGGLE_INFO;
        vrt_refresh();

        mvwprintw(INFOW[BODY], 0, 0, "Portrait");
        mvwprintw(INFOW[BODY], 2, 0, "Given name");
        mvwprintw(INFOW[BODY], 4, 0, "Middle name");
        mvwprintw(INFOW[BODY], 6, 0, "Surname");
        mvwprintw(INFOW[BODY], 8, 0, "Birthplace");
        mvwprintw(INFOW[BODY], 10, 0, "Profession");

        /* Focus and select the portrait field */
        wselect_driver(sel, SEL_FOCUS);
        wselect_driver(sel, SEL_FIRST);

        /* Create the fancy jobs menu */
        jobs = new_fancy_menu(job_list, job_descriptions, PUR_GRE, PUR_PUR);
        put_fancy_menu(jobs, INFOW[BODY], 1, 45, 10, 15);
        set_fancy_detail(jobs, 2, 40, 11, 15, PUR_PUR);
        set_fancy_select(jobs, PUR_YEL);
        post_fancy_menu(jobs);

        i = 0;

	while ((ch = wgetch(INFOW[BODY])) != KEY_ESC) {
                // Handle forms
                if (i == 0) {
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
                        case KEY_DOWN: 
                        case '\n':
                                wselect_driver(sel, SEL_UNFOCUS);
                                form_driver(sheet, REQ_FIRST_FIELD); // Focus forms
                                curs_set(1);
                                i++;
                                break;
                        case KEY_UP:   
                                wselect_driver(sel, SEL_UNFOCUS);
                                tog_fancy_select(jobs);
                                i = 5;
                                break;
                        }
                }
                else if (i > 0 && i < 5) {
                        switch(ch) {
                        case KEY_DOWN:
                        case '\n':
                                form_driver(sheet, REQ_NEXT_FIELD);
                                form_driver(sheet, REQ_END_LINE);
                                i++;
                                if (i == 5) {
                                        curs_set(0);
                                        tog_fancy_select(jobs);
                                };
                                break;
                        case KEY_UP:
                                form_driver(sheet, REQ_PREV_FIELD);
                                form_driver(sheet, REQ_END_LINE);
                                i--;
                                if (i == 0) { 
                                        wselect_driver(sel, SEL_FOCUS);
                                        curs_set(0);
                                }
                                break;
                        case KEY_BACKSPACE:
                                form_driver(sheet, REQ_DEL_PREV);
                                break;
                        default:
                                form_driver(sheet, ch);
                                break;
                        }
                }
                else if (i == 5) {
                        switch(ch) {
                        case 'j':
                                menu_driver(jobs->menu, REQ_DOWN_ITEM);
                                post_fancy_detail(jobs);
                                break;
                        case 'k':
                                menu_driver(jobs->menu, REQ_UP_ITEM);
                                post_fancy_detail(jobs);
                                break;
                        case KEY_DOWN:
                                tog_fancy_select(jobs);
                                wselect_driver(sel, SEL_FOCUS);
                                i = 0;
                                break;
                        case KEY_UP:
                                tog_fancy_select(jobs);
                                curs_set(1);
                                form_driver(sheet, REQ_LAST_FIELD); // Focus forms
                                i--;
                                break;
                        }
                        scr_refresh();
                }
        }
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
