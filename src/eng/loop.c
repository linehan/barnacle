#include "../com/arawak.h"

#include <ev.h>

#include "../gfx/gfx.h"
#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "../noun/noun.h"
#include "../noun/types/boat/boat.h"
#include "../txt/gloss.h"
#include "../gfx/ui/notify.h"
#include "../map/terrain.h"
#include "../map/sweet_flow.h"
#include "fsm.h"
#include "tick.h"
#include "../noun/stats.h"


static bool loop_test_active;

static inline void spinloop(int y, int x, const char *str, int sp)
{
        mvwprintw(CONSOLE_WIN, y, x, "(%c) %s\n", SPINNER(sp), str);
}
static inline void tickloop(int y, int x, const char *str)
{
        mvwprintw(CONSOLE_WIN, y, x, "%s %u\n", str, get_tick());
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
        static int sp;
        if (loop_test_active) {
                spinloop(2, 0, "render_cb", sp++);
                tickloop(8, 0, "Game tick:");
                mvwprintw(CONSOLE_WIN, 9, 0, "Maxi tick: %u", UINT32_MAX);
        }
        tick();
        free_nouns();
        dock_update();
        update_inventory_menu();
        send_delayed_msgs();

        nn("Guy")->update();

        if (oops)
                nn("Dummy")->update();

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
        static int sp;
        if (loop_test_active) {
                spinloop(3, 0, "flow_cb", sp++);
        }
        do_flow(ACTIVE);

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
        static int sp;
        if (loop_test_active) {
                spinloop(4, 0, "move_cb", sp++);
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
        static int sp;
        if (loop_test_active) {
                spinloop(5, 0, "animate_cb", sp++);
        }
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
        static int sp;
        if (loop_test_active) {
                spinloop(6, 0, "iolisten_cb", sp++);
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
        ev_timer flow;      // The water effects
        /*ev_timer move;      // weather state shift (particles)*/
        ev_timer animate;   // map state shift

        ev_io_init(&read, &iolisten_cb, 0, EV_READ);
        ev_init(&render, &render_cb);
        /*ev_init(&move, &move_cb);*/
        ev_init(&animate, &animate_cb);
        ev_init(&flow, &flow_cb);

        render.repeat  = .025;
        /*move.repeat    = .9;*/
        animate.repeat = 3.5;
        flow.repeat    = 0.08;

        ev_io_start(readloop, &read);
        ev_timer_again(execloop, &render);
        /*ev_timer_again(execloop, &move);*/
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

