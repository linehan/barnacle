#include "../com/arawak.h"
#include "error.h"
#include "tick.h"

void abort_report(const char *msg)
{
        endwin();

        printf("An exception occurred at tick %u, suspending execution.\n"
               "The handler reported: \"%s\"\n", get_tick(), msg);

        exit(1);
}

