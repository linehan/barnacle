#ifndef __PERLINTYPES 
#define __PERLINTYPES
typedef struct perlin_map {
        double **box;
        double bound;
        int h;
        int w;
        int len;
} PERLIN;
#endif

void simplex_init(void);
void test_simplex_noise(double sea_level);
PERLIN *get_perlin_map(int h, int w);

