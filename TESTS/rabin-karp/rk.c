#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************
 * RABIN-KARP STRING SEARCHING ALGORITHM
 *
 * This algorithm, created by Michael O. Rabin and Richard M. Karp in 1987,
 * uses a rolling hash to find any one of a set of pattern strings in a text.
 * For text of length n and p patterns of combined length m, its average and
 * best case running time is O(n+m) in space O(p), but its worst-case time is
 * O(nm). It is best suited to matching multiple patterns at once.
 *
 * A hash function is a function which converts every string to a numeric
 * value, called its hash value. The algorithm exploits the fact that if two
 * strings are equal, their hash values are also equal. However, to keep the
 * hash values to a reasonable size, it is likely that we will have to assign
 * some different strings the same hash value, which means that for any match,
 * the algorithm will have to manually verify that it is indeed genuine.
 *
 * To further save time, the hash is computed using a rolling hash, which is
 * a hash function which takes the current hash value as its input, producing
 * the next hash value in much less time. The key to the efficiency of the
 * algorithm, then, is the efficiency of computing the hash values of
 * successive substrings of the text. There are different approaches, and this
 * is an active area of research.
 *
 ******************************************************************************/
#define REHASH(a, b, h) ((((h) - (a)*d) << 1) + (b))

int rk(char *x, int m, char *y, int n)
{
        int d, hx, hy, i, j;

        /* Preprocessing */
        /* computes d = 2^(m-1) with the left-shift operator */
        for (d = i = 1; i<m; i++) {
                d = (d<<1);
        }
        for (hy = hx = i = 0; i<m; i++) {
                hx = ((hx<<1) + x[i]);
                hy = ((hy<<1) + y[i]);
        }

        /* Search */
        j = 0;
        while (j <= (n-m)) {
                if (((hx == hy) && memcmp(x, (y+j), m) == 0))
                        return j;
                hy = REHASH(y[j], y[j+m], hy);
                j++;
        }
}

int main(void)
{
        char *text = "UNIX Network Programming, by W. Richard Stevens";
        char *pattern = "ming";

        int tlen = strlen(text);
        int plen = strlen(pattern);

        int ptr = rk(pattern, plen, text, tlen);

        int i;
        printf("Match @ position %d:\n\n", ptr);
        printf("\t%s\n", text);

        printf("\t%*c", ptr+1, text[ptr]);
        for (i=1; i<plen; i++) {
                printf("%c", text[ptr+i]);
        }
        printf("\n\n");

        return 0;
}
