#include <stdlib.h>
#include <stdio.h>
#include "shuffle.h"


int main(void)
{
        int isrc[10]={1,2,3,4,5,6,7,8,9,10};
        int ishf[10];
        float fsrc[5]={1.0, 8.0, 4.2, 85.221, 93.8};
        float fshf[5];

        int i;

        shuf(isrc, ishf, 10, sizeof(int));
        shuf(fsrc, fshf, 5, sizeof(float));

        for (i=0; i<10; i++)
                printf("isrc[%d]: %d  ishf[%d]: %d\n", i, isrc[i], i, ishf[i]);

        for (i=0; i<5; i++) 
                printf("fsrc[%d]: %f  fshf[%d]: %f\n", i, fsrc[i], i, fshf[i]);

        return 0;
}
