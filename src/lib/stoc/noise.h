#pragma once
#ifndef __STOC_NOISE_H
#define __STOC_NOISE_H


void init_simplex(void);


double **empty_simplex_matrix(uint32_t h, uint32_t w);
double simplex_noise(double yin, double xin);
double **simplex_matrix(int h, int w);
double **tuned_perlin_map(uint32_t h, uint32_t w, double mean);


#endif
