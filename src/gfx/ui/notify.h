#pragma once
#ifndef __UI_NOTIFY_H
#define __UI_NOTIFY_H

#include "../../txt/gloss.h"

struct phrase_t {
        int n; /* Number of phrases in 'say' */
        const char *say[];
};


enum alerts {
        I_FIND,
        I_KILL,
};


void alert(enum alerts tag, char *msg);
void alertf(short co, short hi, const wchar_t *wfmt, ...);
void picksay(const wchar_t *icon, struct phrase_t *phrase);


#endif
