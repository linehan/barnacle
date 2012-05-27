#pragma once
#ifndef __GEOJUG_EXCEPTION_H
#define __GEOJUG_EXCEPTION_H

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#define TRY        do { jmp_buf exception_env__; if (!setjmp(exception_env__)) {
#define CATCH(msg, ...) } else { abort_report(msg, __VA_ARGS__); } } while(0)
#define THROW(x)   longjmp(exception_env__, (x))
/* e.g. ***********************************************************************
 *      char *str = NULL;
 *      TRY {
 *              if (!str);
 *                      THROW(4);
 *      } CATCH("The horror... the horror...");
 ******************************************************************************/

void abort_report(const char *fmt, ...);
void init_signal_handlers(void);

#endif

