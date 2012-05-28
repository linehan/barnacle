#pragma once
#ifndef __ARAWAK_EXCEPTION_H
#define __ARAWAK_EXCEPTION_H
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

/* Exception-handling macros for TRY-THROW-CATCH patterns using longjmp()
 * e.g. ***********************************************************************
 *      char *str = NULL;
 *      TRY {
 *              if (!str);
 *                      THROW(4);
 *      } CATCH("The horror... the horror...");
 ******************************************************************************/
#define TRY        do { jmp_buf exception_env__; if (!setjmp(exception_env__)) {
#define CATCH(msg, ...) } else { abort_report(msg, __VA_ARGS__); } } while(0)
#define THROW(x)   longjmp(exception_env__, (x))


/* Public functions */
void abort_report(const char *fmt, ...);
void init_signal_handlers(void);


#endif

