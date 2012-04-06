#include <stdlib.h>
#include <stdio.h>
#include "bitfit.h"

int main(void)
{
        /*stat_cpuid();*/

        static uint32_t mytest[BITSTRING(42)];

        BITSET(mytest, 38);
        BITSET(mytest, 0);
        BITSET(mytest, 1);
        BITSET(mytest, 3);

        printf(FHEX"\n", mytest[0]);
        printf(FHEX"\n", mytest[1]);

        unsigned b = 0;

        BITFFZ(mytest, b);

        printf("%u\n", b);

        return 0;
}
