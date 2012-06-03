#ifndef _COMMON_H
#define _COMMON_H

/******************************************************************************
 * COMMON INCLUDES
 *
 * All of these blocks are "opt-in", i.e., you would define the reserved
 * macro if you wished to prevent the block it delimits from being 
 * #included. 
 ******************************************************************************/

/**
 * Type-checking
 */
#ifndef __BARNACLE_NO_TYPESTRICT
#include "container_of.h"
#include "check_type.h"
#include "build_assert.h"
#endif


/**
 * C99 features and types
 */
#ifndef __BARNACLE_NO_C99
#include <stdint.h>
#include <stdbool.h>
#endif


/**
 * ncurses libraries. These are required all over the place, obviously.
 * gcc has to be told to compile these in order for them to get linked
 * properly. The library flags are
 *
 * -lmenu -lpanel -lform -lncursesw
 *
 * Note especially the 'w' at the end of lncurses, which enables support
 * for wide-characters. That's pretty important.
 *
 * As for the _XOPEN_SOURCE_EXTENDED macro, it must be defined as soon as 
 * possible, and definitely before the ncurses libraries are included.
 * Without it, most of the operations on wide-character types can start
 * acting up, if they compile at all. YMMV
 *
 * The libc manual sayeth: 
 *
 *       Macro: _XOPEN_SOURCE_EXTENDED
 *
 *   If you define this macro, functionality described in the 
 *   X/Open Portability Guide is included. This is a superset 
 *   of the POSIX.1 and POSIX.2 functionality and in fact 
 *   _POSIX_SOURCE and _POSIX_C_SOURCE are automatically defined.
 *
 *    As the unification of all Unices, functionality only available 
 *    in BSD and SVID is also included.
 */
#ifndef __BARNACLE_NO_NCURSESW

#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED = 1
#endif

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>
#include <wchar.h>
#include <locale.h>

/**
 * The ESC key code is somewhat implementation-defined, and ncurses 
 * doesn't define a macro for it as such, although it supports many 
 * other special keys. '27' is common enough for now, just don't forget!
 */
#define KEY_ESC 27

#endif



/******************************************************************************
 * HASSLE-FREE SPINNERS 
 *
 * e.g.
 *      for (i=0; i<lim; i++)
 *              printf("%c Crunching...\n", SPINNER(i));
 ******************************************************************************/
#define NUM_SPINNER_CHARS 4
static const char SPINNER_CHAR[NUM_SPINNER_CHARS]="|/-\\";
#define SPINNER(indexer) SPINNER_CHAR[((int)(indexer)%NUM_SPINNER_CHARS)]

#define num_wspin 4
static const wchar_t wspin_wch[]=L"◒◐◓◑●";
#define wspin(indexer) wspin_wch[((int)(indexer)%num_wspin)]
#define wspin_solid    wspin_wch[4]




/******************************************************************************
 * THINGS I PILFERED FROM THE LINUX KERNEL
 *
 * Like a freak show of the unfathomable, equal parts lewd and fascinating...
 ******************************************************************************/
/******************************************************************************
 * The __CHECKER__ family of macros provide reminders and diagnostics to 
 * check that your code isn't needlessly wasting time or doing something 
 * silly.
 ******************************************************************************/
#ifdef __CHECKER__
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)
#define BUILD_BUG_ON_ZERO(e) (0)
#define BUILD_BUG_ON_NULL(e) ((void*)0)
#define BUILD_BUG_ON(condition)
#define BUILD_BUG() (0)
#else /* __CHECKER__ */

/**
 * BUILD_BUG_ON - break compile if a condition is true.
 * @condition: the condition which the compiler should know is false.
 *
 * If you have some code which relies on certain constants being equal, or
 * other compile-time-evaluated condition, you should use BUILD_BUG_ON to
 * detect if someone changes it.
 *
 * The implementation uses gcc's reluctance to create a negative array, but
 * gcc (as of 4.4) only emits that error for obvious cases (eg. not arguments
 * to inline functions).  So as a fallback we use the optimizer; if it can't
 * prove the condition is false, it will cause a link error on the undefined
 * "__build_bug_on_failed".  This error message can be harder to track down
 * though, hence the two different methods.
 */
#ifndef __OPTIMIZE__
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#else
extern int __build_bug_on_failed;
#define BUILD_BUG_ON(condition)					\
	do {							\
		((void)sizeof(char[1 - 2*!!(condition)]));	\
		if (condition) __build_bug_on_failed = 1;	\
	} while(0)
#endif


/**
 * BUILD_BUG - break compile if used.
 *
 * If you have some code that you expect the compiler to eliminate at
 * build time, you should use BUILD_BUG to detect if it is
 * unexpectedly used.
 */
#define BUILD_BUG()						\
	do {							\
		extern void __build_bug_failed(void)		\
			__linktime_error("BUILD_BUG failed");	\
		__build_bug_failed();				\
	} while (0)


/** 
 * BUILD_BUG_ON_NOT_POWER_OF_2
 * Force a compilation error if a constant expression is not a power of 2 
 */
#define BUILD_BUG_ON_NOT_POWER_OF_2(n) \
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))


/** 
 * BUILD_BUG_ON_ZERO / NULL 
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted). 
 */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))


/**
 * BUILD_BUG_ON_INVALID 
 * Permits the compiler to check the validity of the expression but 
 * avoids the generation of any code, even if that expression has 
 * side-effects.
 */
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))


#endif	/* __CHECKER__ */



/******************************************************************************
 * TYPE SNOOPING 
 * Try not to let the compiler get away with doing funny things to the wrong
 * types. An art, not a science. 
 ******************************************************************************/

/**
 * Make sure a macro argument is a variable:
 * declare an enumeration inside a new scope with the same name
 * as the variable.
 *
 * Macros for macros, that's right folks.
 */
#define VARIABLE(v) { enum v { }; }

#define ASSIGN(variable, value) \
        VARIABLE(variable) \
        { enum { E = value }; } \
        variable = value;


/** 
 * __same_type
 * Are two types/vars the same type (ignoring qualifiers)?
 */
#ifndef __same_type
# define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#endif


/**
 * __must_be_array
 * In C, you can often get away with thinking about arrays and pointers
 * as two sides of the same coin, but there IS a distinction, and it 
 * tends to sow strange buggies if ignored. 
 *
 * In the macro, &a[0] degrades to a pointer, which is a different type
 * than an array. Usually the compiler will print a warning and move on,
 * but BUILD_BUG_ON_ZERO ensures the whole thing gets called off.
 */
#ifdef __CHECKER__
#define __must_be_array(arr) 0
#else
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#endif


/**
 * ARRAY_SIZE
 * Justly famous macro for determining the size of a dynamic array at
 * runtime. The version from the Linux kernel manages to improve on a
 * classic by stipulating the argument be a true array using the 
 * __must_be_array macro described above.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))


/******************************************************************************
 * OPTIMIZATION
 * Macros that take advantage of (usually) compiler-specific features to
 * attempt to increase the performance of your code. This is usually 
 * unsuccessful, but it has its place.
 ******************************************************************************/

/**
 * Branch prediction macros; gcc will emit instructions causing the branch
 * prediction to favor the instruction on the "likely" side, re-arranging
 * the jumps so that it gets tested first.
 *      e.g.
 *      if (unlikely(c < 4)) {
 *              special code
 *      }
 * There has been some evidence that performance improvements here are
 * negligible in all but the most special cases.
 */
#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif


/******************************************************************************
 * MATHS
 * Preprocessor stand-ins for some of the unsexy functions in <math.h>,
 * plus safe incrementers/decrementers that amount to bounds-checking
 * in most use cases.
 ******************************************************************************/

/**
 * abs() handles unsigned and signed longs, ints, shorts and chars.  For all
 * input types abs() returns a signed long.
 *
 * abs() should not be used for 64-bit types (s64, u64, long long) - use abs64()
 * for those.
 */
#define myabs(x) ({				        \
        long ret;					\
        if (sizeof(x) == sizeof(long)) {		\
                long __x = (x);				\
                ret = (__x < 0) ? -__x : __x;		\
        } else {					\
                int __x = (x);				\
                ret = (__x < 0) ? -__x : __x;		\
        }						\
        ret;						\
	})

#define myabs64(x) ({				        \
	s64 __x = (x);			                \
	(__x < 0) ? -__x : __x;		                \
	})


/**
 * min()/max()/clamp() macros that also do strict type-checking... 
 * See the "unnecessary" pointer comparison.
 */
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define min3(x, y, z) ({			\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	typeof(z) _min3 = (z);			\
	(void) (&_min1 == &_min2);		\
	(void) (&_min1 == &_min3);		\
	_min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) : \
		(_min2 < _min3 ? _min2 : _min3); })

#define max3(x, y, z) ({			\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	typeof(z) _max3 = (z);			\
	(void) (&_max1 == &_max2);		\
	(void) (&_max1 == &_max3);		\
	_max1 > _max2 ? (_max1 > _max3 ? _max1 : _max3) : \
		(_max2 > _max3 ? _max2 : _max3); })


/**
 * min_not_zero - return the minimum that is _not_ zero, unless both are zero
 * @x: value1
 * @y: value2
 */
#define min_not_zero(x, y) ({			\
	typeof(x) __x = (x);			\
	typeof(y) __y = (y);			\
	__x == 0 ? __y : ((__y == 0) ? __x : min(__x, __y)); })


/**
 * clamp - return a value clamped to a given range with strict typechecking
 * @val: current value
 * @min: minimum allowable value
 * @max: maximum allowable value
 *
 * This macro does strict typechecking of min/max to make sure they are of the
 * same type as val.  See the unnecessary pointer comparisons.
 */
#define clamp(val, min, max) ({			\
	typeof(val) __val = (val);		\
	typeof(min) __min = (min);		\
	typeof(max) __max = (max);		\
	(void) (&__val == &__min);		\
	(void) (&__val == &__max);		\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val; })

/**
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max/clamp at all, of course.
 */
#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })


/**
 * clamp_t - return a value clamped to a given range using a given type
 * @type: the type of variable to use
 * @val: current value
 * @min: minimum allowable value
 * @max: maximum allowable value
 *
 * This macro does no typechecking and uses temporary variables of type
 * 'type' to make all the comparisons.
 */
#define clamp_t(type, val, min, max) ({		\
	type __val = (val);			\
	type __min = (min);			\
	type __max = (max);			\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val; })


/**
 * clamp_val - return a value clamped to a given range using val's type
 * @val: current value
 * @min: minimum allowable value
 * @max: maximum allowable value
 *
 * This macro does no typechecking and uses temporary variables of whatever
 * type the input argument 'val' is.  This is useful when val is an unsigned
 * type and min and max are literals that will otherwise be assigned a signed
 * integer type.
 */
#define clamp_val(val, min, max) ({		\
	typeof(val) __val = (val);		\
	typeof(val) __min = (min);		\
	typeof(val) __max = (max);		\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val; })


/**
 * swap - swap value of @a and @b
 */
#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)


/******************************************************************************
 * END LINUX KERNEL NOVELTIES
 ******************************************************************************/
/******************************************************************************
 * BEGIN MISCELLANEOUS MONSTROSITIES
 ******************************************************************************/

/**
 * VA_NUM_ARGS
 * Counts the number of VA_ARGS by means of an intense and heathen magic,
 * the particulars of which are not to be uttered here.
 */ 
#define VA_NUM_ARGS(...) \
        VA_NUM_ARGS_IMPL(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, \
                                      53, 52, 51, 50, 49, 48, 47, 46, 45, 44, \
                                      43, 42, 41, 40, 39, 38, 37, 36, 35, 34, \
                                      33, 32, 31, 30, 29, 28, 27, 26, 25, 24, \
                                      23, 22, 21, 20, 19, 18, 17, 16, 15, 14, \
                                      13, 12, 11, 10,  9,  8,  7,  6,  5,  4, \
                                       3,  2,  1)

#define VA_NUM_ARGS_IMPL( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
                         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                         _61, _62, _63, N, ...) N

/**
 * dec/inc
 * Safe decrement and increment; the value of x is modified
 */
#define dec(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define inc(x, max) x = ((x) < (max)) ? ((x)+1) : (x)




#define NUM_HEADINGS 16
enum hdg_t {NORTH,NNE,NE,ENE,EAST,ESE,SE,SSE,SOUTH,SSW,SW,WSW,WEST,WNW,NW,NNW};        

/**
 * Reserved modes for the FSM and function returns
 */
enum fsm_reserved_modes { 
        MODE_DEFAULT=-2,
        MODE_INITIAL=-3,
        MODE_PERSIST=-4, 
        MODE_RELEASE=-5,
        MODE_RESTORE=-6,
        MODE_STARTED=-7,
        MODE_TYPING=-8,
        NOTOUCH=-9
};


#include <stdlib.h>
/* Because we intend to re-use or re-reference *pptr, the idea of re-lease
 * is appropriate. */
inline static void release(void **pptr)
{
        if (pptr != NULL && *pptr != NULL) { 
                free(*pptr);                     
                *pptr = NULL;                 
        }
}



#endif /* header */
