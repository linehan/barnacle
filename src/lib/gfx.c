// vim:fdm=marker
/* ==========================================================================
    FILENAME:  gfx.c
    This module makes extensive use of the "famous" doubly linked list 
    routines written by Rusty Russell <rusty@rustcorp.com.au> for use in 
    the Linux kernel. He later modified them to an even more general form, 
    which is implemented here. For more information, see the CCAN entry 
    at <http://ccodearchive.net/info/list.html>.
   ========================================================================== */
   #include <stdio.h>
   #include <stdlib.h>
   #include <ncurses.h>
   #include <panel.h>
   #include <pthread.h>
   #include <semaphore.h>

   #include "lib/list/list.h"
   #include "palette.h"
/* ========================================================================== */
/* A structure that bundles all the window dimensions */
   typedef struct dimension_t {
     int h;  /* window height */
     int w;  /* window width  */
     int y0; /* window initial y */
     int x0; /* window initial x */
     int dy; /* window current y */
     int dx; /* window current x */
     int n;  /* number of windows */
   } DIMS;

/* A mobile data type containing some object */
   typedef struct mob_t {
     PANEL *pan;
     DIMS   dim;
     void  *obj;
   } MOB;

/* A node in a linked list ("wad") of windows */
   typedef struct win_wad {
      WINDOW *window;
      struct list_node winnode; 
   } WINNODE;

/* A node in a linked list ("wad") of graphics */
   typedef struct gfx_wad {
      struct list_head winwad; /* a winwad */
      PANEL *pan;
      DIMS   dim;
      struct list_node gfxnode;
   } GFXNODE;

sem_t *REFRESH_LOCK;
/* ========================================================================== */
void gfx_init(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
}
/* --------------------------------------------------------------------------
   Add a new graphics node to a WAD
   -------------------------------------------------------------------------- */
   void add_gfx(GFXNODE *node, struct list_head *head)
   {
     list_add(head, &(node->gfxnode));
   }
/* --------------------------------------------------------------------------
   Initialize a new GFXNODE
   -------------------------------------------------------------------------- */
   GFXNODE *new_gfx(int h, int w, int y0, int x0, int n)
   {
     WINNODE *tmp;
     int i;

     /* Initialize the GFXNODE */
     GFXNODE *gfx = (GFXNODE *)malloc(sizeof(GFXNODE));

     list_head_init(&(gfx->winwad)); /* init the winwad head */

     /* Populate the winwad */
     for (i=0; i<n; i++) {
        WINNODE *new  = (WINNODE *)malloc(sizeof(WINNODE));
        new->window = newwin(h, w, y0, x0);
        list_add(&(gfx->winwad), &(new->winnode)); /* add new to winwad */
     }
     /* Attach the first window of the winwad to the panel */
     tmp = list_top(&(gfx->winwad), WINNODE, winnode);
     gfx->pan = new_panel(tmp->window);

     /* Populate the DIMS with the supplied arguments */
     gfx->dim.h = h;
     gfx->dim.w = w;
     gfx->dim.y0 = y0;
     gfx->dim.x0 = x0;
     gfx->dim.dy = 0;
     gfx->dim.dx = 0;
     gfx->dim.n = n;

     return gfx;
   }
/* --------------------------------------------------------------------------
   Create a MOB
   -------------------------------------------------------------------------- */
   MOB *new_mob(void *ptr, int h, int w, int y0, int x0)
   {
     MOB *mob = (MOB *)malloc(sizeof(MOB));

     mob->obj = ptr;

     WINDOW *win = newwin(h, w, y0, x0);
     mob->pan = new_panel(win);

     mob->dim.h = h;
     mob->dim.w = w;
     mob->dim.y0 = y0;
     mob->dim.x0 = x0;
     mob->dim.dy = 0;
     mob->dim.dx = 0;
     mob->dim.n = 0;

     return mob;
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
/* Master refresh */
void master_refresh(void)
{
        sem_wait(REFRESH_LOCK);
                update_panels();
                doupdate();
        sem_post(REFRESH_LOCK);
}
