
#include <stdarg.h>
#include "../com/arawak.h"

char *mydup(const char *str);
wchar_t *mbdup(const char *str);
wchar_t *wcsdup(const wchar_t *wcs);

void pumpf(char **strp, const char *fmt, ...);
void wpumpf(wchar_t **wcsp, const wchar_t *wfmt, ...);
void wpumpw(WINDOW *win, const wchar_t *wfmt, ...);
