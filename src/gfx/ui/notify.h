#pragma once
#ifndef __UI_NOTIFY_H
#define __UI_NOTIFY_H

#include "../../txt/gloss.h"


enum alerts {
        I_FIND,
        I_KILL,
};


void alert(enum alerts tag, const char *msg);
void alertf(short co, short hi, const wchar_t *wfmt, ...);


#endif
