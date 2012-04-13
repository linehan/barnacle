#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <assert.h>
#include "textutils.h"

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

/*
 * Like mydup(), but for wide-character strings. 
 *
 * SUCCESS: Pointer to new copy
 * FAILURE: NULL
 */
wchar_t *wcdup(const wchar_t *wcs)
{	
        wchar_t *p;
	size_t len;

	len = wcslen(wcs) + 1;
	p   = malloc(len * sizeof (wchar_t));

        return p ? wmemcpy(p, wcs, len) : NULL;
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



        
