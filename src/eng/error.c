#include "../com/arawak.h"
#include "error.h"
#include "tick.h"
#include "../lib/textutils.h"

void abort_report(const char *fmt, ...)
{
        endwin();

        char buf[1000];
        va_list args;

        /* Write formatted output to stream */
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);

        printf("An exception occurred at tick %u, suspending execution.\n"
               "The handler reported: \"%s\"\n", get_tick(), buf);

        exit(1);
}

