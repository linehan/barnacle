/* ==========================================================================
    FILENAME:  gfx.c
    Create the basic graphical element type and perform operations with it.
   ========================================================================== */
   #include <stdio.h>
   #include <stdlib.h>
   #include <ncurses.h>
   #include <panel.h>

   #include "lib/list/list.h"
   #include "palette.h"
/* ========================================================================== */
   typedef struct dimension_t {
     int h;
     int w;
     int y0;
     int x0;
     int dy;
     int dx;
     int n; /* number of windows */
   } DIMS;

   typedef struct win_wad {
      WINDOW *window;
      struct list_node winnode; 
   } WINWAD;

   typedef struct gfx_wad {
      struct list_head winwad;
      PANEL *pan;
      DIMS  *dim;
      struct list_node gfxnode;
   } GFX;

/* ========================================================================== */
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void add_gfx(GFX *mynode, struct list_head *head)
   {
     list_add(head, &(mynode->gfxnode));
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   GFX *new_gfx(int h, int w, int y0, int x0, int n)
   {
     int i;
     WINWAD *tmp;

     GFX *gfx = (GFX *)malloc(sizeof(GFX));

     list_head_init(&(gfx->winwad));
     for (i=0; i<n; i++) {
          WINWAD *new  = (WINWAD *)malloc(sizeof(WINWAD));
          new->window = newwin(h, w, y0, x0);
          list_add(&(gfx->winwad), &(new->winnode));
     }
     tmp = list_top(&(gfx->winwad), WINWAD, winnode); 
     gfx->pan = new_panel(tmp->window);

     /* the dimensions that were passed are preserved for later */
     gfx->dim     = (DIMS *)malloc(sizeof(DIMS));
     gfx->dim->h  = h;
     gfx->dim->w  = w;
     gfx->dim->y0 = y0;
     gfx->dim->x0 = x0;
     gfx->dim->dy = 0;
     gfx->dim->dx = 0;
     gfx->dim->n  = n;

     return gfx;
   }
/* --------------------------------------------------------------------------
   Swap a panel
   -------------------------------------------------------------------------- */
   /*
   void animate_gfxlist(GFXLIST *ptr)
   {
     GFXLIST *tmp = ptr;
     while (tmp->next != NULL) {
       tmp->win = tmp->win->next;
       replace_panel(tmp->pan, tmp->win->window);
       tmp = tmp->next;
     }
   }
   */
