/* 
 * notify.c -- Handle requests to print on the dock buffer.
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <assert.h>
#include "../../com/barnacle.h"
#include "../../txt/gloss.h"
#include "../../lib/textutils.h"
#include "../../lib/stoc/stoc.h"
#include "dock.h"
#include "notify.h"
#include "../../test/test.h"



/*
 * alert -- create a gloss object to do the talking 
 * @msg: the message to be used in the gloss object
 */
void alert(enum alerts tag, const char *msg)
{
        wchar_t wcs[200];

        wclean(wcs, 200);

        switch (tag) {
        case I_FIND:
                swpumpf(wcs, 200, L"Found %s", msg);
                say_alert(wcs, __GOLD);
                break;
        case I_KILL:
                swpumpf(wcs, 200, L"Killed %s!", msg);
                say_alert(wcs, __KILL);
                break;
        }
}


/*
 * alert -- create a gloss object to do the talking 
 * @msg: the message to be used in the gloss object
 */
void alertf(short co, short hi, const wchar_t *wfmt, ...)
{
        va_list args;
        wchar_t buf[200];
        wclean(buf, 200);

        /* Write formatted output to stream */
        va_start(args, wfmt);
        vswprintf(buf, 200, wfmt, args);
        va_end(args);

        say_alert(buf, co, hi);
}


