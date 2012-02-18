/******************************************************************************
 *
 *   O, it's a snug little island!
 *   A right little, tight little island!
 *      -Thomas Dibdin, "The Snug Little Island".
 *
 *   Ay, many flowering islands lie
 *   In the waters of wide Agony.
 *       -Percy Bysshe Shelley, "Lines written among the Euganean Hills"
 *
 * ---------------------------------------------------------------------------
 *
 * We have a 2D array of elements that we are going to consider in a binary
 * fashion, that is, each element in the 2D array can only have one of two
 * states. You can imagine a square where some pixels are black and some are
 * white. In our case, an element/pixel/box/cell is either land, or water.
 *
 * How can we determine whether and how many of these elements are contiguous,
 * so that we can consider them as bits of a larger "landmass" or "image",
 * instead of individual little pips. There are lots of ways to do this, and
 * as it turns out, this is a very interesting and useful question, despite
 * its humble application here. 
 *
 * It's called "connected component analysis", and while there is a decent
 * selection of algorithms to choose from, most of them perform two passes:
 * one to label the components, and another to compare the labels. The one 
 * we're going to use is a bit slicker, essentially an implementation of 
 * a new-ish single-pass approach.[1]
 *
 * The single-pass algorithm decides like this. Consider a grid of binary
 * elements, such as the one below.
 *
 * How should we find out which elements are contiguous? We will classify each
 * as belonging to some set, and give that set a unique index number.
 * Every time we find a new '#', it is assigned an index number. Contiguous 
 * '#'s should have the same index number, though, because they belong to the
 * same set. That's the knot we're about to open.
 *
 *    ----------------   We can do this in one scan, one row at a time,
 *    #-##----##----#-   we only need a few things:
 *    ----#-#--#---#--
 *    #-###-#----#----     1. Our current row position x.
 *    --#-#-#--#--##--     2. A buffer that holds the set indexes of the
 *    ----###-----#---        previously scanned row (BUF[x]).
 *    ---------#------     3. The set index of the cell we just scanned, 
 *    ------------##--        i.e., the one just to our left (LEFT).
 *                         4. A tolerant disposition.
 *
 * As we scan across from left to right, at each cell, we assign it a set
 * number by following these simple rules:
 *
 *                           ( Am I '#', whatever that is)
 *                                         |
 *                                         |
 *                            +----yes----[?]----no-----------------------+ 
 *                            |                                           |
 *                            V                                           |
 *                      (LEFT == 0)                                       |
 *                            |                                           |
 *                            |                                           |
 *              +-----yes----[?]----no---------+                          |
 *              |                              |                          |
 *              V                              V                          |
 *      (BUF[x] == 0)                  (BUF[x] == 0)                      |
 *              |                              |                          |
 *              |                              |                          |
 *     +--yes--[?]--no--+          +----yes---[?]--no--+                  |
 *     |                |          |                   |                  |
 *     |                |          |                   V                  |   
 *     |                |          |           (BUF[x] == LEFT)           |
 *     |                |          |                   |                  |
 *     |                |          |                   |                  |
 *     |                |          |          +--yes--[?]--no--+          |
 *     |                |          |          |                |          |
 *     *                *          *          *                *          *
 *    NEW             BUF[x]      LEFT     LEFT/BUF     (LEFT(U)BUF[x])   0
 *
 * 
 * Try it yourself using the little diagram at the top. No peeking ahead,
 * remember, you can only see yourself, one row above you, and one cell 
 * behind you. Study carefully the cells which result in a union, such as 
 * (4,5), and, even more dramatically, (6,7). Brings a tear to my eye.
 *
 * ---------------------------------------------------------------------------
 *
 * [1] D. Bailey and C. Johnston, "Single pass connected components analysis"
 *     in Image and Vision Computing New Zealand, 2008, pp. 282-287
 *
 ******************************************************************************/
typedef struct set_t { 
        int id;
        int n;
        int y0;
        int x0;
        int y;
        int x;
} MEMBER;
/******************************************************************************
 * Make a new member
 ******************************************************************************/
MEMBER *coin(int y, int x, int setcount)
{
        MEMBER *new = (MEMBER *)malloc(sizeof(MEMBER));
        new->id = setcount;
        new->n  = 1;
        new->y0 = y;
        new->y  = y;
        new->x0 = x;
        new->x  = x;

        return new;
}
/******************************************************************************
 * Add the current cell to an existing set.
 ******************************************************************************/
void addto(MEMBER *set, int y_coord, int x_coord)
{
        if (set == NULL) {
                speak_error("FUCKYOU");
                return;
        }
        /* We have a new member! */
        set->n++;
        /* Our ant is always moving left --> right and top --> bottom. 
        * This means that when we want to add our coordinates to a 
        * set, we can be damn sure that we have the biggest y coord
        * (if the cell we are touching is to the north) or, at least, 
        * one just as big (to the west).
        *
        * However, we can't be so sure about our x coordinate, since all 
        * kinds of strange shapes may have existed in long-forgotten 
        * buffers overhead. Moral of the story: we have to compare our x 
        * value, but we can always write our y value without bothering to 
        * check if it's >= or not, because it is. */
        set->y = y_coord;
        set->x = (set->x <= x_coord) ? x_coord : set->x;
        /* What should also be obvious, but perhaps isn't, is that the y0
        * and x0 values need not even be considered by this function,
        * because by definition, the cells about which our ant can reason
        * are cells with coordinates of (x-1) and (y-1). His new information
        * cannot possibly overturn the set's x0 and y0 values. */
}
/******************************************************************************
 * Arrange the union of two sets
 ******************************************************************************/
int unite(MEMBER *set1, MEMBER *set2)
{
        /* It makes no effective difference which set is "left standing"
        * for our current implementation, so the order in which these
        * arguments are passed can be arbitrary. But, because some of the
        * operations done below are one-sided, we have to pick a convention.
        * The rule is: the one with the smallest 'id' number gets to be
        * the parent. */
        MEMBER *dom, *sub;
        if (set1->id < set2->id) {
                dom = set1;
                sub = set2;
        }
        else {
                dom = set2;
                sub = set1;
        }

        sub->id = dom->id;

        /* We sum the number of elements */
        dom->n += sub->n;

        /* We must merge x0 and y0 values, preserving the lowest ones,
         * because they're minima. */
        dom->y0 = (dom->y0 >= sub->y0) ? sub->y0 : dom->y0;
        dom->x0 = (dom->x0 >= sub->x0) ? sub->x0 : dom->x0;

        /* We do the same for the x/y values, except we preserve the 
         * highest, because they're maxima. */
        dom->x = (dom->x <= sub->x) ? sub->x : dom->x;
        dom->y = (dom->y <= sub->y) ? sub->y : dom->y;

        /* Any future references to the child set will be re-routed to the 
        * parent set instead, and we emit the index of this parent set, so
        * that it can become the new index of both sets from now on. Also,
        * tradition insists that the index be proclaimed "flex supreme, 
        * worldwide champion of the world". */
        sub = dom;
        return dom->id;
}
/******************************************************************************
 * Connected components analysis
 ******************************************************************************/
void connected_components(PLATE *pl, int type)
{
        PERLIN *pmap = pl->noise;

        int i, y, x, h, w;

        int tag  = 0;
        int left = 0;
        int above[pmap->w];

        for (i=0; i<(pmap->w); i++) {
                above[i] = 0;
        }

        /* A lookup table (array of pointers to sets) */
        MEMBER *set[pmap->len];

        for (y=0; y<(pmap->h); y++) {
                for (x=0; x<(pmap->w); x++) {
                        /* If current cell is land */
                        if ((pmap->box[y][x] > pmap->bound)) {
                                /* Cell to the left isn't land */
                                if (left == 0) {
                                        /* Cell above isn't land */
                                        if (above[x] == 0) {
                                                /*It's a new set */
                                                tag += 1;
                                                set[tag] = coin(y, x, tag);
                                                above[x] = tag;
                                                    left = tag;
                                                /*wprintw(BIGWIN, "%d", tag);*/
                                        }
                                        /* Cell above is land */
                                        else {
                                                /* Add to set above[x] */
                                                addto(set[above[x]], y, x);
                                                left = above[x];
                                                /*wprintw(BIGWIN, "%d", above[x]);*/
                                        }
                                }
                                /* Left is land, above isn't */ 
                                else if (above[x] == 0) {
                                        /* Add to set 'left' */
                                        addto(set[left], y, x);
                                        above[x] = left;
                                        /*wprintw(BIGWIN, "%d", left);*/
                                }
                                /* Both above and left are the same land */
                                else if (above[x] == left) {
                                        /* Add to either set, e.g. 'left' */
                                        addto(set[left], y, x);
                                        /*wprintw(BIGWIN, "%d", left);*/
                                }
                                /* Above and left think they are different */
                                else {
                                        /* Unite left and above */
                                        left = unite(set[left], set[above[x]]);
                                        above[x] = left;
                                        addto(set[left], y, x);
                                        /*wprintw(BIGWIN, "M");*/
                                }
                        }
                        /* If current cell isn't land (so lonely...) */
                        else { 
                                left = 0;
                                above[x] = 0;
                                /*wprintw(BIGWIN, "~");*/
                        }
                }
                /*wprintw(BIGWIN, "\n");*/
        }
        /* Once we are all done */
        int prev = 0;
        /* For each of our counted tags */
                        wprintw(BIGWIN, "number of tags: %d\n", tag);

        for (i=1; i<tag; i++) {
                wprintw(BIGWIN, "%d ", set[i]->id);
        }
        for (i=1; i<tag; i++) {

                /* If the set id is unique */
                if (set[i]->id >= prev) {
                        /* Create an appropriate GNODE for that set */
                        gen_terrain(pl, 'm', set[i]->y, set[i]->x, set[i]->y0, set[i]->x0);
                        prev = set[i]->id;
                        wprintw(BIGWIN, "and a GNODE for: %d\n", set[i]->id);
                }
        }
           
}
