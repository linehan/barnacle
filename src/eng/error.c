/* 
 * error.c -- errors, signal reporting/handling, and exceptions
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "../com/barnacle.h"
#include "../lib/textutils.h"
#include "tick.h"
#include "error.h"


/* EXCEPTION HANDLING
``````````````````````````````````````````````````````````````````````````````*/
/**
 * abort_report -- clean up and exit, printing a brief diagnostic report
 * @fmt: a printf-style format string
 * @...: the variable argument list to the format string
 */
void abort_report(const char *fmt, ...)
{
        char buf[1000];
        va_list args;

        endwin(); /* End curses mode */

        /* Write formatted output to stream */
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);

        printf("An exception occurred at tick %u, suspending execution.\n"
               "The handler reported: \"%s\"\n", get_tick(), buf);

        exit(1);
}


/* SIGNAL HANDLING
``````````````````````````````````````````````````````````````````````````````*/
/**
 * handle_signal_simple -- clean up and exit on receipt of certain signals
 * @signo: signal number
 */
void handle_signal_simple(int signo)
{
        endwin(); /* End curses mode */

        switch (signo) {
        case SIGINT:
                printf("Received signal %d (SIGINT) at tick %u\n", 
                        signo, get_tick());
                break;
        case SIGSEGV:
                printf("Received signal %d (SIGSEGV) at tick %u\n",
                       signo, get_tick());
                break;
        case SIGABRT:
                printf("Received signal %d (SIGABRT) at tick %u\n",
                       signo, get_tick());
                break;
        case SIGFPE:
                printf("Received signal %d (SIGFPE) at tick %u\n",
                       signo, get_tick());
                break;
        case SIGTERM:
                printf("Received signal %d (SIGTERM) at tick %u\n",
                       signo, get_tick());
                break;
        default:
                printf("Received signal %d at tick %u\n",
                       signo, get_tick());
                break;
        }

        exit(1);
}

/**
 * handle_signal_verbose -- clean up and exit on receipt of certain signals
 * @signo  : signal number
 * @info   : information about what generated the signal
 * @context: the process context when the signal was raised
 */
void handle_signal_verbose(int signo, siginfo_t *info, void *context)
{
        handle_signal_simple(signo);
}

/****************************************************************************** 
 * signal.h defines the sigaction() function:
 *
 * int sigaction(int sig, const struct sigaction *restrict act,
 *                              struct sigaction *restrict oact);
 *
 * where 'act' specifies the implementation-defined signal handling, and
 * 'oact' refers to the location at which the default signal handling
 * configuration will be stored. These are of type struct sigaction, which
 * is also defined in signal.h. See man(3) signal.h
 ******************************************************************************/
/* The new signal handling configuration */
const struct sigaction new_action = {
        .sa_handler   = &handle_signal_simple,
        .sa_mask      = 0,
        .sa_flags     = 0,
        .sa_sigaction = &handle_signal_verbose,
};
/* The old (default) signal handling configuration */
struct sigaction old_action = {0};


/**
 * init_signal_handlers -- provide the handler functions to the system 
 */
void init_signal_handlers(void)
{
        sigaction(SIGINT,  &new_action, &old_action);
        sigaction(SIGSEGV, &new_action, &old_action);
        sigaction(SIGABRT, &new_action, &old_action);
}


