#pragma once
#ifndef __ARAWAK_INIT_H
#define __ARAWAK_INIT_H


#define urxvt_resources (\
        "Xft*antialias: true"                                     \
        "Xft*hinting:   true"                                     \
        "Xft*hintstyle: hintfull"                                 \
        "Xft*rgba:      rgb"                                      \
        "URxvt.font:    xft:DejaVu Sans Mono:pixelsize=14,"       \
        "               xft:Unifont:pixelsize=16:antialias=false")

int arawak_init(void);


#endif
