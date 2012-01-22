#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([WHALEROAD], [0.0.2], [jdl@flexagon.homeunix.org])
AC_CONFIG_SRCDIR([SRC/whaleroad.c])
AM_INIT_AUTOMAKE([WHALEROAD], [0.0.2])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lev':
AC_CHECK_LIB([ev], [ev_timer_again])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [floor])
# FIXME: Replace `main' with a function in `-lmenu':
AC_CHECK_LIB([menu], [new_menu])
# FIXME: Replace `main' with a function in `-lncursesw':
AC_CHECK_LIB([ncursesw], [newwin])
# FIXME: Replace `main' with a function in `-lpanel':
AC_CHECK_LIB([panel], [new_panel])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_join])

# Checks for header files.
AC_CHECK_HEADERS([locale.h stddef.h stdlib.h string.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([setlocale])

AC_CONFIG_FILES([Makefile SRC/Makefile])
AC_OUTPUT
