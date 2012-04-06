#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_RECUR  300

void quicksort(uint32_t *arr, int elements) 
{
        uint32_t piv, swap;       // Comparison values
        uint32_t beg[MAX_RECUR]; // Less than pivot
        uint32_t end[MAX_RECUR]; // Greater than pivot
        int i=0, L, R;            // Incrementors

        beg[0] = 0; 
        end[0] = elements;

        while (i>=0) {
                L=beg[i]; 
                R=end[i]-1;

                if (L<R) {
                        piv = arr[L];
                        while (L<R) {
                                while (arr[R]>=piv && L<R) R--; 
                                if (L<R) 
                                        arr[L++] = arr[R];
                                while (arr[L]<=piv && L<R) L++; 
                                if (L<R) 
                                        arr[R--] = arr[L];
                        }
                        arr[L]=piv;
                        beg[i+1]=L+1;
                        end[i+1]=end[i];
                        end[i++]=L;

                        if (end[i]-beg[i]>end[i-1]-beg[i-1]) {
                                swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
                                swap=end[i]; end[i]=end[i-1]; end[i-1]=swap;
                        }
                }
                else i--; 
        }     
}
