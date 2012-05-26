#include "../com/arawak.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

#include "textutils.h"
#include "../gfx/gfx.h"



/*
 * Allocates memory appropriate to duplicate the char * string pointed
 * to by 'str'. 
 *
 * SUCCESS: Pointer to new copy
 * FAILURE: NULL
 */
char *mydup(const char *str)
{
        char *p;
        size_t len;

        len = strlen(str) + 1;
        p   = malloc(len);

        return p ? memcpy(p, str, len) : NULL;
}


/*	$OpenBSD: wcsdup.c,v 1.1 2011/07/04 04:37:34 nicm Exp $	*/
/*	$NetBSD: wcsdup.c,v 1.3 2008/05/26 13:17:48 haad Exp $	*/

/*
 * Copyright (C) 2006 Aleksey Cheusov
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee. Permission to modify the code and to distribute modified
 * code is also granted without any restrictions.
 */
wchar_t *wcsdup(const wchar_t *str)
{
	wchar_t *copy;
	size_t len;

	len = wcslen(str) + 1;
	copy = malloc(len * sizeof(wchar_t));

	if (!copy)
		return (NULL);

	return (wmemcpy(copy, str, len));
}


wchar_t *mbdup(const char *str)
{
        assert(str!=NULL);

        wchar_t *p;
        size_t len;

        len = strlen(str) + 1;
        assert(len >= mbstowcs(NULL, str, 0)+1);
        /*
         * If the assert is not met, it means that non-L'\0'  
         * wide characters have been stored at dest.  
         * In this case the number of wide characters written 
         * to dest is returned, but the shift state at this 
         * point is lost. (see mbstowcs(3), item #2)
         */
        p = malloc(len * sizeof (wchar_t));

        if (p!=NULL) mbstowcs(p, str, len);
        
        return p ? p : NULL;
}




/**
 * pumpf -- write a formatted character string into an auto-allocated buffer
 * @strp : pointer to a character buffer (will be allocated)
 * @fmt  : format string
 * @...  : format string arguments
 * @ret  : length of the formatted string at *strp
 */
void pumpf(char **strp, const char *fmt, ...) 
{
        va_list args;
        size_t len;
        FILE *stream;

        /* Open a new FILE stream. *strp will be dynamically allocated to
         * contain characters written to the stream, and len will reflect
         * these changes. See man(3) open_memstream. */
        stream = open_memstream(strp, &len);
        assert(stream || !"pumpf: Unable to open FILE stream");

        /* Write formatted output to stream */
        va_start(args, fmt);
        vfprintf(stream, fmt, args);
        va_end(args);

        fflush(stream);
        fclose(stream);

        if (!*strp)
                return -ENOMEM;
}       



/**
 * wpumpf -- write a formatted wchar_t string into an auto-allocated buffer
 * @wcsp : pointer to a wide-character buffer (will be allocated)
 * @wfmt : wide-character format string
 * @...  : format string arguments
 * @ret  : length of the formatted wide-character string at *wcsp
 */
void wpumpf(wchar_t **wcsp, const wchar_t *wfmt, ...)
{
        va_list args;
        size_t len = 0;
        FILE *stream;

        /* Open a new FILE stream. *wcsp will be dynamically allocated to
         * contain wide-characters written to the stream, and len will 
         * reflect these changes. See man(3) open_wmemstream. */
        stream = open_wmemstream(wcsp, &len);
        assert(stream || !"wpumpf: Unable to open FILE stream");

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vfwprintf(stream, wfmt, args);
        va_end(args);

        fflush(stream);
        fclose(stream);

        if (!*wcsp)
                return -ENOMEM;
}


/**
 * wpumpf -- write a formatted wchar_t string into an auto-allocated buffer
 * @win  : WINDOW pointer (ncurses)
 * @wfmt : wide-character format string
 * @...  : format string arguments
 */
void wpumpw(WINDOW *win, const wchar_t *wfmt, ...)
{
        #define WPUMPW_MAXLEN 300 
        wchar_t buf[WPUMPW_MAXLEN];
        va_list args;

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vswprintf(buf, WPUMPW_MAXLEN, wfmt, args);
        va_end(args);

        waddwstr(win, buf);
}


/**
 * mvwpumpw -- call wpumpw after moving the cursor 
 * @win  : WINDOW pointer (ncurses)
 * @yof  : y offset 
 * @xof  : x offset
 * @wfmt : wide-character format string
 * @...  : format string arguments
 */
void mvwpumpw(WINDOW *win, int y, int x, const wchar_t *wfmt, ...)
{
        #define WPUMPW_MAXLEN 300 
        wchar_t buf[WPUMPW_MAXLEN];
        va_list args;

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vswprintf(buf, WPUMPW_MAXLEN, wfmt, args);
        va_end(args);

        wmove(win, y, x);
        waddwstr(win, buf);
}


/**
 * mvcwpumpw -- call mvwpumpw after changing the window rendition 
 * @win  : WINDOW pointer (ncurses)
 * @yof  : y offset 
 * @xof  : x offset
 * @wfmt : wide-character format string
 * @...  : format string arguments
 */
void mvcwpumpw(WINDOW *win, short pair, int y, int x, const wchar_t *wfmt, ...)
{
        #define WPUMPW_MAXLEN 300 
        wchar_t buf[WPUMPW_MAXLEN];
        va_list args;
        size_t len;

        /* Write formatted output to stream */
        va_start(args, wfmt);
        len = vswprintf(buf, WPUMPW_MAXLEN, wfmt, args);
        va_end(args);

        wmove(win, y, x);
        waddwstr(win, buf);
        mvwchgat(win, y, x, len, 0, pair, NULL);
}


/**
 * swpumpf -- write a formatted wchar_t string into a pre-allocated buffer
 * @buf  : wchar_t buffer (must be allocated already)
 * @wfmt : wide-character format string
 * @...  : format string arguments
 * @ret  : length of the formatted wide-character string
 */
size_t swpumpf(wchar_t *buf, size_t maxlen, const wchar_t *wfmt, ...)
{
        va_list args;
        size_t len;

        assert(buf || !"swpumpf buffer must be previously allocated");

        /* Write formatted output to stream */
        va_start(args, wfmt);
        len = vswprintf(buf, maxlen, wfmt, args);
        va_end(args);

        return len;
}




