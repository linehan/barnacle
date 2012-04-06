#include <stdlib.h>
#include <stdbool.h>
#include <ev.h>

#include "../gfx/gfx.h"
#include "../gfx/ui/dock.h"
#include "../map/map.h"
#include "../mob/boat/boat_model.h"
#include "../mob/boat/boat_control.h"
#include "../mob/boat/boat_view.h"
#include "../verb/verb_view.h"
#include "txt/gloss.h"

#include "../lib/stoc/stoc.h"
#include "fsm.h"

////////////////////////////////////////////////////////////////////////////////
void
refresh_cb(EV_P_ ev_timer *w, int revents)
{
        vrt_refresh();
        scr_refresh();
        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void
print_cb(EV_P_ ev_timer *w, int revents)
{
        static bool loop_ready;
        static struct gloss_t *test;

        if (!loop_ready || test == NULL) {
                loop_ready = (roll_fair(40)==2) ? true : false;
                if (!loop_ready) return;
                else
                        test = rand_gloss(dock_window(SUBJ_TX_WIN));
        }

        loop_ready = say(test);

        scr_refresh();
        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void 
move_cb(EV_P_ ev_timer *w, int revents)
{
        sail_boat(get_boat("Afarensis"));
        restack_map(GLOBE);
        map_refresh(GLOBE);

        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void 
sail_cb(EV_P_ ev_timer *w, int revents)
{
        draw_compass();
        approach_helm();

        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void 
weather_cb(EV_P_ ev_timer *w, int revents)
{
        /*mark_wind();*/
        /*seek_prevailing();*/
        view_dock();
        do_pulse();
        draw_compass();
        surface_flow(GLOBE, 50);
        restack_map(GLOBE);
        map_refresh(GLOBE);

        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void 
animate_cb(EV_P_ ev_timer *w, int revents)
{
        NEXT(GLOBE->L[RIM]);
        restack_map(GLOBE);
        map_refresh(GLOBE);

        ev_timer_again(EV_DEFAULT, w);
}


////////////////////////////////////////////////////////////////////////////////
void 
*iolisten(EV_P_ ev_io *w, int revents)
{
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
        ev_timer sail;      // boat state shift 
        ev_timer weather;   // weather state shift
        ev_timer animate;   // map state shift
        ev_timer print;     // prints text effects
        ev_timer refresh;   // write the new state to the screen
        ev_timer move;

        ev_io_init(&read, &iolisten, 0, EV_READ);
        ev_init(&sail, &sail_cb);
        ev_init(&weather, &weather_cb);
        ev_init(&animate, &animate_cb);
        ev_init(&print, &print_cb);
        ev_init(&refresh, &refresh_cb);
        ev_init(&move, &move_cb);

        move.repeat    = .08;
        sail.repeat    = .02;
        weather.repeat = .08;
        animate.repeat = 3.5;
        print.repeat   = .02;
        refresh.repeat = .01;

        ev_io_start(readloop, &read);
        ev_timer_again(execloop, &sail);
        ev_timer_again(execloop, &weather);
        ev_timer_again(execloop, &animate);
        ev_timer_again(drawloop, &print);
        ev_timer_again(drawloop, &refresh);
        ev_timer_again(drawloop, &move);

        ev_run(execloop, 0);
        ev_run(drawloop, 0);

        return 0;
}
