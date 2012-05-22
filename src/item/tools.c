#include <stdlib.h>
#include "../com/arawak.h"
#include "../map/cell.h"
#include "../map/terrain.h"
#include "../lib/stoc/stoc.h"
#include "../noun/models.h"
#include "item.h"
#include "tools.h"

/* Ropes
``````````````````````````````````````````````````````````````````````````````*/
struct rope_t {
        int len;
        struct cell_t *head;
        struct cell_t *tail;
};

/* NEW */
void new_rope(void *self)
{
        #define ROPE_S_ICON L"∮"
        #define ROPE_M_ICON L"∯"
        #define ROPE_L_ICON L"∰"
        #define ROPE_L_LEN 9 

        ITEM_OBJECT(item, self);

        struct rope_t *new;

        new = malloc(sizeof(struct rope_t));

        new->len  = ROPE_L_LEN;
        new->head = NULL;
        new->tail = NULL;

        item->data = new;
        item->tag  = ITEM_ROPE;
        item->name = "Rope";
        item->desc = "for roping dopes";
        item->icon = ROPE_L_ICON;
        item->use  = &use_rope;
}


/* USE */
void use_rope(void *self, struct noun_t *noun)
{
        struct item_t *item;
        struct rope_t *rope;
        int tile;
        int i;
        int y;
        int next;
        int x;

        if (pos_hdg(noun->pos) != WEST 
        &&  pos_hdg(noun->pos) != EAST)
                return;

        item = (struct item_t *)self;
        rope  = (struct rope_t *)item->data;
        y     = pos_y(noun->pos);
        x     = pos_x(noun->pos);

        /* Decide which way the rope will dangle */
        if (pos_hdg(noun->pos) == WEST) {
                tile = L_ROPE_ANCHOR;
                DEC(x, 0);
        }
        else if (pos_hdg(noun->pos) == EAST) {
                tile = R_ROPE_ANCHOR;
                INC(x, COLS);
        }

        /* Don't draw anything if the tile isn't the floor */
        if (TILE(ACTIVE, y, x) != CAVEFLOOR)
                return;

        rope->head = new_cell(y, x);
        rope->tail = rope->head; /* Whoa */

        SET_TILE(ACTIVE, y, x, tile);

        /* Unwind the rope */
        for (i=0; i<rope->len; i++) {

                struct cell_t *new;

                INC(y, LINES);
                /* Look ahead */
                if (TILE(ACTIVE, ADDONE(y, LINES), x) == CAVESOLID)
                       break; 
                
                new         = new_cell(y, x);
                new->parent = rope->tail;
                rope->tail  = new;

                SET_TILE(ACTIVE, y, x, ROPE);
        }
        MAPBOOK->render(ACTIVE);
}


/* Pickaxe 
``````````````````````````````````````````````````````````````````````````````*/

void new_pickaxe(void *self)
{
        #define PICKAXE_WCH L"ང"

        ITEM_OBJECT(item, self);

        item->data = NULL;
        item->tag  = ITEM_PICKAXE;
        item->name = "Pickaxe";
        item->icon = PICKAXE_WCH;
        item->use  = &use_pickaxe;
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
                noun_set_signal(noun, SM_DigUp, '*');
                break;
        case SOUTH:
                INC(y, LINES);
                noun_set_signal(noun, SM_DigDown, '*');
                break;
        case EAST:
                INC(x, COLS);
                noun_set_signal(noun, SM_DigRight, '*');
                break;
        case WEST:
                DEC(x, 0);
                noun_set_signal(noun, SM_DigLeft, '*');
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

        ITEM_OBJECT(item, self);

        item->data = NULL;
        item->tag  = ITEM_SHOVEL;
        item->name = "Shovel";
        item->icon = SHOVEL_WCH;
        item->use  = &use_shovel;
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
                noun_set_signal(noun, SM_DigDown, '*');
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


void equip_torch(void *self, bool yn);
void burn_torch(void *self, struct noun_t *noun);


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

        ITEM_OBJECT(item, self);
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

        new->win = newwin(new->h, new->w, 0, 0);
        new->pan = new_panel(new->win);
        hide_panel(new->pan);

        new->lit = false;

        item->data  = new;
        item->tag   = ITEM_TORCH;
        item->icon  = TORCH_WCH;
        item->name  = "Torch";
        item->burn  = &burn_torch;
        item->equip = &equip_torch;
}


void equip_torch(void *self, bool yn)
{
        ITEM_OBJECT(item, self);
        struct torch_t *torch = (struct torch_t *)item->data;

        item->equipped = yn;

        if (item->equipped)
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

        ITEM_OBJECT(item, self);
        struct torch_t *torch = item->data;

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




