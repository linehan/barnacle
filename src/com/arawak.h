#ifndef _COMMON_H
#define _COMMON_H

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

#define KEY_ESC 27 /* ESC keycode */

#define TOGPAN(pan)              \
        if (panel_hidden(pan))   \
                show_panel(pan); \
        else    hide_panel(pan)  

#define CENTERED ((LINES)/2), ((COLS)/2)
#endif




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
        NOTOUCH=-8
};


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

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


#ifdef __CHECKER__
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)
#define BUILD_BUG_ON_ZERO(e) (0)
#define BUILD_BUG_ON_NULL(e) ((void*)0)
#define BUILD_BUG_ON(condition)
#define BUILD_BUG() (0)
#else /* __CHECKER__ */

/* Force a compilation error if a constant expression is not a power of 2 */
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)			\
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))

/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))

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

#endif	/* __CHECKER__ */


#endif /* header */
