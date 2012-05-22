#ifndef _COMMON_H
#define _COMMON_H

/*
 * Branch prediction macros; gcc will emit instructions causing the branch
 * prediction to favor the instruction on the "likely" side, re-arranging
 * the jumps so that it gets tested first.
 *      e.g.
 *      if (unlikely(c < 4)) {
 *              special code
 *      }
 */
#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif


/*
 * The next three blocks are opt-in, so you would define in order to *prevent*
 * them being #included.
 */
#ifndef __ARAWAK_NO_TYPESTRICT__
#include "container_of.h"
#include "check_type.h"
#include "build_assert.h"
#endif


#ifndef __ARAWAK_NO_C99__
#include <stdint.h>
#include <stdbool.h>
#endif

/*
 * You need to compile these with -lmenu -lpanel -lform -lncursesw for them
 * to be linked properly.
 *
 * gcc expects the #define and will not properly render UTF-8 support without
 * it, so it had better appear before any #include statements.
 */
#ifndef __ARAWAK_NO_NCURSESW__
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>
#include <wchar.h>
#include <locale.h>



/*
 * The ESC key code is somewhat implementation-defined, and
 * ncurses does not define a macro for it as such, although
 * it supports many other special keys. '27' is common enough.
 */
#define KEY_ESC 27


#define TOGPAN(pan)              \
        if (panel_hidden(pan))   \
                show_panel(pan); \
        else    hide_panel(pan)  

#define CENTERED ((LINES)/2), ((COLS)/2)
#endif



/* Macros for macros ... yes that's right folks */

/*
 * Make sure a macro argument is a variable:
 * declare an enumeration inside a new scope with the same name
 * as the variable.
 */
#define VARIABLE(v) { enum v { }; }

#define ASSIGN(variable, value) \
        VARIABLE(variable) \
        { enum { E = value }; } \
        variable = value;


/*
 * e.g.
 *      for (i=0; i<lim; i++)
 *              printf("%c Crunching...\n", SPINNER(i));
 */
#define NUM_SPINNER_CHARS 4
static const char SPINNER_CHAR[NUM_SPINNER_CHARS]="|/-\\";
#define SPINNER(indexer) SPINNER_CHAR[((int)(indexer)%NUM_SPINNER_CHARS)]



/*
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


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])) 







/*
 * abs() handles unsigned and signed longs, ints, shorts and chars.  For all
 * input types abs() returns a signed long.
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


/*
 * min()/max()/clamp() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define MIN(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define MAX(x, y) ({				\
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

/*
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


/*
 * swap - swap value of @a and @b
 */
#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)


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

/*
 * Safe decrement and increment
 */
#define DEC(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define INC(x, max) x = ((x) < (max)) ? ((x)+1) : (x)
#define BETWEEN(x, min, max) ((x) > (min) && (x) < (max)) ? true : false

#define ADDONE(x, max) ((x) < (max)) ? ((x)+1) : (x)
#define SUBONE(x, min) ((x) > (min)) ? ((x)-1) : (x)

#define NUM_HEADINGS 16
enum hdg_t {NORTH,NNE,NE,ENE,EAST,ESE,SE,SSE,SOUTH,SSW,SW,WSW,WEST,WNW,NW,NNW};        

static const char *hdg_tag[NUM_HEADINGS]={"NORTH","NNE","NE","ENE","EAST","ESE",
                                          "SE","SSE","SOUTH","SSW","SW","WSW",
                                          "WEST","WNW","NW", "NNW"};


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
