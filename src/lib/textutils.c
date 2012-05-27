#include "../com/arawak.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <wchar.h>
#include <wctype.h>
#include <string.h>
#include <assert.h>

#include "textutils.h"
#include "../gfx/gfx.h"


/* ALLOCATED COPY AND CONVERSION
``````````````````````````````````````````````````````````````````````````````*/
/**
 * cdup -- copy *str to a newly-alloc'd buffer, and return a pointer to it 
 *
 * @str: pointer to a '\0'-terminated char string
 *  RET: pointer to a copy of *str, else NULL.
 */
char *cdup(const char *str)
{
        char *copy;
        size_t len;

        len  = strlen(str) + 1;
        copy = malloc(len);

        return copy ? memcpy(copy, str, len) : NULL;
}




/**
 * wdup -- copy *wcsto a newly-alloc'd buffer, and return a pointer to it 
 *
 * @wcs: pointer to a L'\0'-terminated wchar_t string
 *  RET: pointer to a copy of *wcs, else NULL.
 */
wchar_t *wdup(const wchar_t *wcs)
{
	wchar_t *copy;
	size_t len;

	len  = wcslen(wcs) + 1;
	copy = malloc(len * sizeof(wchar_t));

	return copy ? wmemcpy(copy, wcs, len) : NULL;
}




/**
 * mbdup -- like wdup, but converts str to a multi-byte character string 
 * 
 * @str: pointer to a '\0'-terminated character string
 *  RET: pointer to a copy of *str, converted to muti-byte characters
 */
wchar_t *mbdup(const char *str)
{
        wchar_t *copy;
        size_t len;
        size_t mblen;

        len   = strlen(str) + 1;
        mblen = mbstowcs(NULL, str, 0) + 1;

        assert(len >= mblen || !"Not a pure character string");
        /*
         * If the assert is not met, it means that non-L'\0'  
         * wide characters have been stored at dest.  
         * In this case the number of wide characters written 
         * to dest is returned, but the shift state at this 
         * point is lost. (see mbstowcs(3), item #2)
         */
        copy = malloc(len * sizeof(wchar_t));

        if (copy) 
                mbstowcs(copy, str, len);
        
        return copy ? copy : NULL;
}

/* HELPERS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * wclean -- given a wide-character buffer, set the contents to L'\0'
 * @wcs : pointer to a wide-character buffer
 * @len : size of the wide-character buffer
 */
void wclean(wchar_t *wcs, size_t len)
{
        wmemset(wcs, L'\0', len);
}


/**
 * cclean -- given a character buffer, set the contents to '\0'
 * @str : pointer to a character buffer
 * @len : size of the character buffer
 */
void cclean(char *str, size_t len)
{
        memset(str, '\0', len);
}



/* FORMATTED PRINTING 
``````````````````````````````````````````````````````````````````````````````*/
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
        size_t len;
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
        static const size_t maxlen = 300;
        wchar_t *buf;
        va_list args;

        buf = calloc(maxlen, sizeof(wchar_t));

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vswprintf(buf, maxlen, wfmt, args);
        va_end(args);

        waddwstr(win, buf);

        free(buf);
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
        static const size_t maxlen = 300;
        wchar_t *buf;
        va_list args;

        buf = calloc(maxlen, sizeof(wchar_t));

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vswprintf(buf, maxlen, wfmt, args);
        va_end(args);

        wmove(win, y, x);
        waddwstr(win, buf);

        free(buf);
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
        static const size_t maxlen = 300;
        wchar_t *buf;
        va_list args;
        size_t len;

        buf = calloc(maxlen, sizeof(wchar_t));

        /* Write formatted output to stream */
        va_start(args, wfmt);
        len = vswprintf(buf, maxlen, wfmt, args);
        va_end(args);

        wmove(win, y, x);
        waddwstr(win, buf);
        mvwchgat(win, y, x, len, 0, pair, NULL);

        free(buf);
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




