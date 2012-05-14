#include "../com/arawak.h"

#include <ev.h>

#include "../gfx/gfx.h"
#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "../noun/noun.h"
#include "../noun/types/boat/boat.h"
#include "../verb/verb.h"
#include "../txt/gloss.h"
#include "../map/terrain.h"
#include "../map/sweet_flow.h"
#include "fsm.h"


static bool loop_test_active;


/* -------------------------------------------------------------------------- */
void
print_cb(EV_P_ ev_timer *w, int revents)
{
        static bool loop_ready;
        static struct gloss_t *test;

        if (!loop_ready || test == NULL) {
                loop_ready = (roll_fair(40)==2) ? true : false;
                if (!loop_ready) return;
                else
                        test = rand_gloss(dock_window(TEXT_WIN));
        }

        loop_ready = say(test);

        scr_refresh();
        ev_timer_again(EV_DEFAULT, w);
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
        static int spindex;
        struct noun_t *noun;
        if (loop_test_active) {
                mvwprintw(CONSOLE_WIN, 1, 0, "(%c) render_cb\n", 
                          SPINNER(spindex++));
        }
        /*view_dock();*/
        /*draw_compass();*/
        /*approach_helm();*/
        dock_update();
        /*noun_update(get_noun("Dummy"));*/
        noun = get_noun("Guy");
        noun_update(noun);

        noun->fall(noun);

        update_panels();  /* Were slowing down performance, unnecessary */
        doupdate();

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
        static int spindex;
        if (loop_test_active) {
                mvwprintw(CONSOLE_WIN, 2, 0, "(%c) flow_cb\n", 
                          SPINNER(spindex++));
        }
        render_sweet_flow(ACTIVE);

        MAPBOOK->restack(ACTIVE);
        map_refresh(ACTIVE);

        ev_timer_again(EV_DEFAULT, w);
}

/* -------------------------------------------------------------------------- */

/*
 * move_cb
 *
 * Called by the movement loop, this callback shifts the position of entities
 * on-screen. Its timing is the closest thing to the "heartbeat" of the game, 
 * if only because the player is most aware of its effects.
 * Repeat: .08 seconds
 */
void move_cb(EV_P_ ev_timer *w, int revents)
{
        static int spindex;
        if (loop_test_active) {
                mvwprintw(CONSOLE_WIN, 3, 0, "(%c) move_cb\n", 
                          SPINNER(spindex++));
        }
        /*do_pulse();*/
        /*noun_render(get_noun("Afarensis"));*/

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
        static int spindex;
        if (loop_test_active) {
                mvwprintw(CONSOLE_WIN, 4, 0, "(%c) animate_cb\n", 
                          SPINNER(spindex++));
        }
        NEXT(ACTIVE->L[RIM]);
        tab_update();

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
        static int spindex;
        if (loop_test_active) {
                mvwprintw(CONSOLE_WIN, 5, 0, "(%c) iolisten_cb\n", 
                          SPINNER(spindex++));
        }
        ev_io_stop (EV_A, w);

        int ch = getch();

        if (ch == 'q') {
                ev_break(EV_A_ EVBREAK_ALL);
                return NULL;
        }
        director(ch);

        ev_io_start(EV_A, w);
}


////////////////////////////////////////////////////////////////////////////////
int start_event_watchers(void)
{
        struct ev_loop *readloop = EV_DEFAULT;
        struct ev_loop *execloop = EV_DEFAULT;
        struct ev_loop *drawloop = EV_DEFAULT;

        ev_io read;         // stdin is readable
        ev_timer render;    // boat state & compass state
        ev_timer flow;      /* The water effects */
        ev_timer move;      // weather state shift (particles)
        ev_timer animate;   // map state shift

        ev_io_init(&read, &iolisten_cb, 0, EV_READ);
        ev_init(&render, &render_cb);
        ev_init(&move, &move_cb);
        ev_init(&animate, &animate_cb);
        ev_init(&flow, &flow_cb);

        render.repeat  = .025;
        move.repeat    = .08;
        animate.repeat = 3.5;
        flow.repeat    = .1;

        ev_io_start(readloop, &read);
        ev_timer_again(execloop, &render);
        ev_timer_again(execloop, &move);
        ev_timer_again(execloop, &animate);
        ev_timer_again(execloop, &flow);

        ev_run(execloop, 0);
        ev_run(drawloop, 0);

        return 0;
}


void loop_test(void)
{
        if (loop_test_active) {
                loop_test_active = false;
                werase(CONSOLE_WIN);
        } else {
                loop_test_active = true;
                mvwprintw(CONSOLE_WIN, 0, 0, "Event loop status\n");
        }
}

