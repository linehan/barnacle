#include "../com/arawak.h"

#include <ev.h>

#include "../gfx/gfx.h"
#include "../noun/noun.h"
#include "../map/map.h"
#include "../map/sweet_flow.h"
#include "../test/looptest.h"
#include "fsm.h"
#include "tick.h"


/* -------------------------------------------------------------------------- */
/*
 * pause_loop
 *
 * Will run until the pause key ('p') is pressed again, at which time the
 * loop will be broken and the 'p' sent to the master FSM, where it will
 * be switched to disable the game_is_paused boolean (see globals.h) 
 */
void pause_loop(void)
{
        int ch;
        while (ch=getch(), ch != 'p') {
                pause();
                print_paused();
                napms(25);
        }
        director(ch);
}


/* -------------------------------------------------------------------------- */
/*
 * render_cb 
 *
 * Called by the rendering loop, which is the tightest of the event loops. 
 * It writes any modified state to the screen, and tries not to calculate much
 * itself, since it has to return quickly.
 * Repeat: .02 seconds
 */
void render_cb(EV_P_ ev_timer *w, int revents)
{
        spin_render_loop();

        if (game_is_paused)
                pause_loop();

        tick();

        free_nouns();
        update_nouns();

        dock_update();
        update_inventory_menu();

        print_dock();
        update_panels();  /* Were slowing down performance, unnecessary */
        doupdate();
        MAPBOOK->restack(ACTIVE);
        map_refresh(ACTIVE);

        ev_timer_again(EV_DEFAULT, w);
}


/* -------------------------------------------------------------------------- */
/*
 * flow_cb
 *
 * This callback calculates the next iteration of the surface flow and renders
 * them on the screen.
 * Repeat: 0.1 seconds
 */
void flow_cb(EV_P_ ev_timer *w, int revents)
{
        spin_flow_loop();

        do_flow(ACTIVE);

        ev_timer_again(EV_DEFAULT, w);
}


/* -------------------------------------------------------------------------- */
/*
 * animate_cb
 *
 * Probably an ambiguous name, this is a long-term callback which steps 
 * through the WINDOWs in a multiwin linked list, drawing the newly active
 * WINDOW to the screen. It is responsible for the edge effects along the
 * shoreline right now.
 * Repeat: 3.5 seconds
 */
void animate_cb(EV_P_ ev_timer *w, int revents)
{
        spin_animate_loop();
        NEXT(ACTIVE->L[RIM]);

        ev_timer_again(EV_DEFAULT, w);
}


/* -------------------------------------------------------------------------- */
/*
 * iolisten_cb
 *
 * Listens for readable input on stdin, triggering when some is detected.
 * So it is responsible for handling user input events, and passing the
 * keypress from the stdin input buffer to the control handler in the FSM.
 * Repeat: n/a
 */
void *iolisten_cb(EV_P_ ev_io *w, int revents)
{
        ev_io_stop (EV_A, w);

        spin_io_loop();

        int ch = getch();

        if (ch == 'q') {
                ev_break(EV_A_ EVBREAK_ALL);
                return NULL;
        }
        director(ch);

        ev_io_start(EV_A, w);
}


/**
 * enter_event_loop -- begin execution of the game loop
 * 
 * After initialization and set-up, this is where the flow of control will
 * reside for the vast majority of the process lifetime. It will only be
 * exited in order to handle a termination signal, or as a response to a
 * user's "quit" command.
 */
void enter_event_loop(void)
{
        #define REPEAT_PERIOD  0.025f
        #define ANIMATE_PERIOD 3.5f
        #define FLOW_PERIOD    0.08f

        struct ev_loop *readloop = EV_DEFAULT;
        struct ev_loop *execloop = EV_DEFAULT;

        ev_io    read;    // stdin is readable
        ev_timer render;  // things which must occur once per tick ("atomic") 
        ev_timer flow;    // water and weather effects, some physics 
        ev_timer animate; // long-period animations of the map window 

        ev_io_init(&read, &iolisten_cb, 0, EV_READ);
        ev_init(&render, &render_cb);
        ev_init(&animate, &animate_cb);
        ev_init(&flow, &flow_cb);

        render.repeat  = REPEAT_PERIOD;
        animate.repeat = ANIMATE_PERIOD;
        flow.repeat    = FLOW_PERIOD;

        ev_timer_again(execloop, &render);
        ev_timer_again(execloop, &animate);
        ev_timer_again(execloop, &flow);

        loop_test(true);

        ev_io_start(readloop, &read);
        ev_run(execloop, 0);
}







