#pragma once
#ifndef __COLORTEST_H
#define __COLORTEST_H


enum {_R_, _G_, _B_};

#define SPEC6 6
#define SPEC11 11

#define TRIPLET(ramp, lvl) ramp[lvl][_R_], ramp[lvl][_G_], ramp[lvl][_B_]

static const uint32_t spec6[SPEC6][3]={ {835, 243, 310},
                                           {988, 553, 349},
                                           {996, 878, 545},
                                           {902, 961, 596},
                                           {600, 835, 580},
                                           {196, 533, 741} };


static const uint32_t spec11[SPEC11][3]={ { 620,    4,  259},
                                          { 835,  243,  310},
                                          { 957,  427,  263},
                                          { 992,  682,  380},
                                          { 996,  878,  545},
                                          {1000, 1000,  749},
                                          { 902,  961,  596},
                                          { 671,  867,  643},
                                          { 400,  761,  647},
                                          { 196,  533,  741},
                                          { 369,  310,  635} };

PANEL *colortest_panel(void);
void paint_ramp(int opt);


#endif
