// vim:fdm=marker
#include <stdlib.h>
#include <stdio.h>

/****************************************************************************** 
 * THE BOYER-MOORE STRING SEARCH ALGORITHM                                 {{{1
 *
 *
 *     "Old Tom Bombadil is a merry fellow,
 *      Bright blue his jacket is, and his boots are yellow
 *      None has ever caught him yet, for Tom, he is the master:
 *      His songs are stronger songs, and his feet are faster."
 * 
 *                                                                       
 * The classic string searching algorithm created in 1977 by Bob Boyer and
 * J. Strother Moore is still  widely considered the most efficient string
 * searching algorithm yet proven.
 *
 * The key innovation is this: the algorithm operates from right to left, 
 * that is, it compares against the last letter of the desired string, not 
 * the first, and also does no pre-processing of the text being searched at
 * all.  Why do it this way?
 *
 * Imagine we wish to search a mystery text for the word "VENEZUELA".
 *
 *         Our query:   VENEZUELA
 *      Mystery text:   ????????????????????????????????????????
 *
 * The naiive approach would be to compare the first letter of VENEZUELA with
 * the first letter of the mystery text. If it's a V, then check the second
 * letter to see if it's an E. If the first letter isn't a V, then move to 
 * the second letter of the mystery text, and see if that one is a V, etc.
 *
 *                      VENEZUELA
 *      NO MATCH        | 
 *                      S???????????????????????????????????????
 *
 *                     ->VENEZUELA
 *      NO MATCH         |
 *                      SH??????????????????????????????????????
 *
 *                     -->VENEZUELA
 *      NO MATCH          |
 *                      SHI?????????????????????????????????????
 *
 * It can be seen that the longer the mystery text, the longer (in general)
 * it's going to take to find the search string, since this algorithm checks
 * every letter of the mystery text up to and including the suspected match.
 *
 * Now let's look at how Boyer-Moore approaches the same problem, by matching
 * the last letter of the search query:
 *
 *                      VENEZUELA
 *      NO MATCH                |
 *                      ????????F???????????????????????????????
 *
 * Obviously, if the last letter doesn't match, it is impossible that any of
 * the previous letters would be part of the search query. How convenient, we
 * knocked out a chunk at the beginning, but we still have to crawl down the
 * string one by one from here, right? 
 *
 * Of course not, this is a legendary algorithm, not some corner-cutting hack,
 * show some respect!
 *
 * The algorithm's next step after finding a mismatch is to check if any of
 * the other letters in the search query match the letter uncovered. In the
 * best case scenario, shown here, the uncovered letter does not occur at all 
 * in the search query. So what do we do?
 *
 *                     --------->VENEZUELA
 *      NO MATCH                         | 
 *                      ????????F????????Z??????????????????????
 *
 * We can skip the next 8 letters, just as we skipped the first 8, knowing
 * with absolute certainty that none of them could have contained the coveted
 * last letter of our query.
 *
 * Now, we find a Z, which doesn't match the last letter, but it DOES occur
 * in our query, and so we calculate how many places to jump next, so that
 * the letter Z in "VENEZUELA" and the Z we've uncovered are aligned. Once
 * that is done, we can again uncover the letter under the last letter of 
 * our query, to see if it matches.
 *
 *                     ------------->VENEZUELA
 *      MATCH!                           |   |
 *                      ????????F????????Z???A??????????????????
 * 
 * Now this is looking good, let's see if the next letter matches the L in
 * VENEZUELA.
 *
 *                     ------------->VENEZUELA
 *      NO MATCH                         |  ||
 *                      ????????F????????Z??RA??????????????????
 *
 * We could grumble, but hey, at least we can skip down another 8 characters
 * and try again! Eventually we'll find VENEZUELA. 
 *
 * Having visualized the way this algorithm does its business, you should
 * immediately recognize its most maniacal-laugh-inducing feature: the longer
 * the search query, the *faster* it will be able to find a match!
 *
 *      "Good. Use your mathematics, boy. Let the adoration 
 *       flow through you."
 *
 * "Titin", also known as "connectin", is a protein responsible for the 
 * elasticity of musicle tissue, among other things. Titin really puts the 
 * poly- in "polypeptide", being the largest such molecule known. Accordingly, 
 * its IUPAC name (a systematic nomenclature for chemical compounds generated 
 * by a set of rules laid out by the International Union of Pure and Applied 
 * chemisty) is also the longest. In fact, it is the longest attested word in
 * any language, ever. Reproducing it is beyond the reasonable scope of this
 * documentation -- it is 189,819 letters long. Consider it the upper bound
 * on your single-word efficiency for this algorithm, given a text of arbitrary
 * length.
 *                                                                         }}}1
 ******************************************************************************/
/******************************************************************************
 * We will implement the TURBO flavor of the BM algorithm, which has a number
 * of advantages:
 *       
 *      + No extra pre-processing
 *      + Searches text in O(n) time
 *      + 2n text comparisons in the worst case
 *
 * Let's get on with it.
 ******************************************************************************/
#define ALPHABET_LEN 255
#define NOT_FOUND plen 
#define MAX(a, b) ((a<b) ? b : a)

/* Create the array bmBAD[] such that bmBAD[c] contains the distance between 
 * the last character of the search pattern and the rightmost occurence of a 
 * letter c in the pattern. If such a letter c does not occur in the pattern,
 * the value of bmBAD[c] = plen (the length of the pattern). 
 *
 * If c is at string[i] and c != pat[plen-1], we can safely shift i over by
 * bmBAD[c]. */
void make_bmBAD(char *pat, int plen, int bmBAD[]) 
{
        int i;

        for (i=0; i<ALPHABET_LEN; i++) {
                bmBAD[i] = plen;
        }
        for (i=0; i<(N-1); ++i) {
                bmBAD[pat[i]] = plen-i-1;
        }
}
/* Length of the longest suffix of the word ending on word[pos]. */
void is_suffix(char *pat, int plen, int *suff)
{
        int f, g, i;

        suff[m-1] = m;
        g = m-1;
        for (i=(m-2); i>=0; --i) {
                if ( (i>g) && (suff[i+m-1-f]<(i-g)) )
                        suff[i] = suff[i+m-1-f];
                else {
                        if (i<g)
                                g = i;
                        f = i;
                        while (g >= 0 && c[g] == c[g+m-1-f])
                                --g;
                        suff[i] = (f-g);
                }
        }
}
void PREbmGOOD(char *c, int m, int bmGOOD[])
{
        int i, j, suff[XSIZE];

        suffixes(c, m, suff);

        for (i=0; i<m; ++i) {
                bmGOOD[i] = m;
        }
        j=0;
        for (i=(m-1); i>=0; --i) {
                if (suff[i] == (i+1)) {
                        for (; j<(m-1-i); ++j) {
                                if ((bmGOOD[j] == m))
                                        bmGOOD[j] = (m-1-i);
                        }
                }
        }
}

void tbm(char *x, int m, char *y, int n) {
        int shift, BADshift, TURBOshift;
        int bmGOOD[XSIZE], bmBAD[ASIZE];
        int i, j, u, v;

        /* Preprocess */
        PREbmGOOD(c, m, bmGOOD);
        PREbmBAD(c, m, bmBAD);

        /* Search the text */
        j = u = 0;
        shift = m;
        while (j <= (n-m)) {
                i = (m-1);
                while (i >= 0 && x[i] == y[i+j]) {
                        --i;
                        if ( (u != 0) && (i == (m-1-shift)) )
                                i -= u;
                }
                if (i<0) {
                        OUTPUT(j);
                        shift = bmGOOD[0];
                        u = (m-shift);
                }
                else {
                        v = (m-1-i);
                        TURBOshift = (u-v);
                        BADshift = (bmBAD[y[i+j]] - (m+1+i));
                        shift = MAX(TURBOshift, BADshift);
                        shift = MAX(shift, bmGOOD[i]);
                        if (shift == bmGOOD[i])
                                u = MIN((m-shift), v);
                        else {
                                if (TURBOshift < BADshift)
                                        shift = MAX(shift, (u+1));
                                u=0;
                        }
                }
                j += shift;
        }
}


