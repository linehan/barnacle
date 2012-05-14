#include <stdlib.h>
#include "../com/arawak.h"
#include "../map/cell.h"
#include "../map/terrain.h"
#include "../lib/stoc/stoc.h"
#include "../noun/models.h"
#include "equipment.h"
#include "rope.h"

#define EQUIPMENT(declare, argument) \
        struct equip_t *declare = (struct equip_t *)argument

/* Ropes
``````````````````````````````````````````````````````````````````````````````*/
struct rope_t {
        int len;
        struct cell_t *head;
        struct cell_t *tail;
};


void new_rope(void *self)
{
        #define ROPE_S_WCH L"∮"
        #define ROPE_M_WCH L"∯"
        #define ROPE_L_WCH L"∰"
        #define ROPE_S_LEN 3
        #define ROPE_M_LEN 6 
        #define ROPE_L_LEN 9 

        EQUIPMENT(equip, self);

        struct rope_t *new;

        new = malloc(sizeof(struct rope_t));

        new->len  = ROPE_L_LEN;
        new->head = NULL;
        new->tail = NULL;

        equip->data = new;
        equip->tag  = ITEM_ROPE;
        equip->name = "Rope";
        equip->wch  = ROPE_L_WCH;
        equip->use  = &use_rope;
}


void use_rope(void *self, struct noun_t *noun)
{
        int tile;
        int i;
        int y;
        int y_ahead;
        int x;

        EQUIPMENT(equip, self);
        struct rope_t *rope = (struct rope_t *)equip->data;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        if (TILE(ACTIVE, (y+1), (x-1)) == CAVEFLOOR) {
                tile = L_ROPE_ANCHOR;
                DEC(x, 0);
        }
        else if (TILE(ACTIVE, (y+1), (x+1)) == CAVEFLOOR) {
                tile = R_ROPE_ANCHOR;
                INC(x, COLS);
        }

        rope->head = new_cell(y, x);
        rope->tail = rope->head;

        SET_TILE(ACTIVE, rope->head->y, rope->head->x, tile);

        for (i=0; i<rope->len; i++) {
                y = (rope->tail->y < LINES) ? rope->tail->y+1 : rope->tail->y;
                y_ahead = (y < LINES) ? y+1 : y;
                x = rope->tail->x;

                if (IS_TILE(ACTIVE, y_ahead, x, CAVESOLID))
                        break;
                
                struct cell_t *new = new_cell(y, x);
                new->parent = rope->tail;
                rope->tail = new;

                SET_TILE(ACTIVE, rope->tail->y, rope->tail->x, ROPE);
        }
        MAPBOOK->render(ACTIVE);
}


/* Pickaxe 
``````````````````````````````````````````````````````````````````````````````*/

void new_pickaxe(void *self)
{
        #define PICKAXE_WCH L"ང"

        EQUIPMENT(equip, self);

        equip->data = NULL;
        equip->tag  = ITEM_PICKAXE;
        equip->name = "Pickaxe";
        equip->wch  = PICKAXE_WCH;
        equip->use  = &use_pickaxe;
}

void use_pickaxe(void *self, struct noun_t *noun)
{
        int tile;
        int y;
        int x;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        switch (noun->pos->hdg) {
        case NORTH:
                DEC(y, 0);
                set_animation(noun, &dig_u_test);
                break;
        case SOUTH:
                INC(y, LINES);
                set_animation(noun, &dig_d_test);
                break;
        case EAST:
                INC(x, COLS);
                set_animation(noun, &dig_r_test);
                break;
        case WEST:
                DEC(x, 0);
                set_animation(noun, &dig_l_test);
                break;
        }

        if (TILE(ACTIVE, y, x) == CAVESOLID) {
                SET_TILE(ACTIVE, y, x, CAVERUBBLE);
        }

        MAPBOOK->render(ACTIVE);
}        

/* Shovel 
``````````````````````````````````````````````````````````````````````````````*/

void new_shovel(void *self)
{
        #define SHOVEL_WCH L"⏚"

        EQUIPMENT(equip, self);

        equip->data = NULL;
        equip->tag  = ITEM_SHOVEL;
        equip->name = "Shovel";
        equip->wch  = SHOVEL_WCH;
        equip->use  = &use_shovel;
}

void use_shovel(void *self, struct noun_t *noun)
{
        int tile_d;
        int y;
        int x;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        tile_d = TILE(ACTIVE, (y+1), x);

        if (tile_d == CAVESOLID) {
                SET_TILE(ACTIVE, (y+1), x, CAVERUBBLE);
                set_animation(noun, &dig_d_test);
        }

        MAPBOOK->render(ACTIVE);
}        

/* Torch 
``````````````````````````````````````````````````````````````````````````````*/

struct torch_t {
        WINDOW *win;
        PANEL *pan;
        bool lit;
        bool blend;
        int h;
        int w;
        int hr;
        int wr;
        short pair[3];
        short bg[3];
        short fg[3];
};


void new_torch(void *self)
{
        #define TORCH_H 5
        #define TORCH_W 7
        #define TORCH_Hr 2
        #define TORCH_Wr 3
        #define TORCH_WCH L"༈"
        static const short WOOD_pair[3] = {LIGHTP1, LIGHTP2, LIGHTP3};
        static const short WOOD_bg[3]   = {LIGHTB1, LIGHTB2, LIGHTB3};
        static const short WOOD_fg[3]   = {LIGHT1, LIGHT2, LIGHT3};

        EQUIPMENT(equip, self);
        struct torch_t *new;
        int i;

        new = calloc(1, sizeof(struct torch_t));

        new->h  = TORCH_H;
        new->w  = TORCH_W;
        new->hr = TORCH_Hr;
        new->wr = TORCH_Wr;

        for (i=0; i<3; i++) {
                new->pair[i] = WOOD_pair[i];
                new->fg[i]   = WOOD_fg[i];
                new->bg[i]   = WOOD_bg[i];
        }

        blend(new->fg[0], 0.2, __DGREY, 1.0, new->fg[2]);
        blend(new->fg[0], 0.5, __DGREY, 1.0, new->fg[1]);
        blend(new->fg[0], 1.0, __DGREY, 1.0, new->fg[0]);
        blend(__DGREY, 0.3, BLACK, 1.0, new->bg[2]);
        blend(__DGREY, 0.5, BLACK, 1.0, new->bg[1]);
        blend(__DGREY, 1.0, BLACK, 1.0, new->bg[0]);

        new->win = newwin(new->h, new->w, 0, 0);
        new->pan = new_panel(new->win);
        hide_panel(new->pan);

        new->lit = false;

        equip->data = new;
        equip->tag  = ITEM_TORCH;
        equip->wch  = TORCH_WCH;
        equip->name = "Torch";
        equip->use = &use_torch;
        equip->burn = &burn_torch;
}


void use_torch(void *self, struct noun_t *noun)
{
        EQUIPMENT(equip, self);
        struct torch_t *torch = equip->data;

        torch->lit ^= true;

        if (torch->lit)
                show_panel(torch->pan);
        else
                hide_panel(torch->pan);
}


void burn_torch(void *self, struct noun_t *noun)
{
        int y;
        int x;
        int i;
        int j;
        int x0;
        int y0;
        int torch_h;
        int torch_w;

        EQUIPMENT(equip, self);
        struct torch_t *torch = equip->data;

        if (!torch->lit)
                return;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        /* -------------------------------------- y-adjustments */
        if (y < torch->hr+1) {
                torch_h = (torch->hr + y);
                y0 = torch->hr - y;
        }
        else if (y > LINES-(torch->hr+1)) {
                torch_h = torch->hr + (LINES-y);
                y0 = y - torch->hr;
        }
        else {
                torch_h = torch->h;
                y0 = y - torch->hr;
        }
        /* -------------------------------------- x-adjustments */
        if (x < torch->wr) 
        {
                torch_w = (torch->wr + x);
                x0 = torch->wr - x;
        }
        else if (x > COLS-torch->wr) {
                torch_w = torch->wr+ (COLS-x);
                x0 = x - torch->wr;
        }
        else {
                torch_w = torch->w;
                x0 = x - torch->wr;
        }

        if (wresize(torch->win, torch_h, torch_w));
                replace_panel(torch->pan, torch->win);

        move_panel(torch->pan, y0, x0);

        /* -------------------------------------- copy bg to torch buffer */
        copywin(PLATE(ACTIVE, BGR), torch->win, y0, x0, 0, 0, torch_h-1, torch_w-1, 0);
        overwrite(noun->win, torch->win);

        /* -------------------------------------- re-color the copied wchars */
        for (i=0; i<torch_h; i++) {
        for (j=0; j<torch_w; j++) {
                if ((i==0 && j==0)
                ||  (i==0 && j==1)
                ||  (i==0 && j==torch_w-1)
                ||  (i==0 && j==torch_w-2)
                ||  (i==torch_h-1 && j==0)
                ||  (i==torch_h-1 && j==1)
                ||  (i==torch_h-1 && j==torch_w-1)
                ||  (i==torch_h-1 && j==torch_w-2))
                {
                        mvwchgat(torch->win, i, j, 1, 0, torch->pair[2], NULL);
                }
                else if (i==0 || i==torch_h-1 || j==0 || j==1 || j==torch_w-1 || j==torch_w-2)
                        mvwchgat(torch->win, i, j, 1, 0, torch->pair[1], NULL);
                else 
                        mvwchgat(torch->win, i, j, 1, 0, torch->pair[0], NULL);
        }
        }
}

