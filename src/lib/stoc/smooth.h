#pragma once
#ifndef __SMOOTH_H
#define __SMOOTH_H

enum smooth_options { SMOOTH_LO, SMOOTH_HI, SMOOTH_BO };

void perlin_smooth(double **pmap, int h, int w, float persist, int oct);
void  smooth_cycle(double **pmap, int h, int w, float lim, int opt, int cycle);
void    smooth_hoz(double **pmap, int h, int w, float lim, int opt);
void    smooth_vrt(double **pmap, int h, int w, float lim, int opt);
double **fit_char(int h, int w);

#endif
