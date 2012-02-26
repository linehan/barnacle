// vim:fdm=marker
#include <stdint.h>
#include <ncurses.h>
#include "../redblack/rb.h"
#include "sw.h"

/*
  Allocate memory for a new struct sw_t, and initialize it.
*/
struct sw_t *new_sw(const char **fields, const char ***options)
{
        struct sw_t *new = malloc(sizeof *new);

        new->tree = NULL;
        new->nib = fields;
        new->opt = options;
        new->c = 0;

        return (new);
}
/*
  Retreive a cell with key 'z', and set its 'n'th nibble to state 's'.
*/
//{{{1 Detailed explanation
//##############################################################################
//#                                                                            #
//#   TILE   0010 0011 0100 1110 1111 \__                                      #
//#  SCRUB   1111 0000 1111 1111 1111 /  \                                     #
//#          |||| |||| |||| |||| ||||    AND                                   #
//#  CLEAN   0010 0000 0100 1110 1111 \__                                      #
//#  STATE   0000 1001 0000 0000 0000 /  \                                     #
//#          |||| |||| |||| |||| ||||    OR                                    #
//#  FINAL   0010 1001 0100 1110 1111                                          #
//#                                                                            #
//#                                                                            #
//#  state[opt]    the hex bitmask indexed by opt, (an enum)                   #
//#  offset[tag]   the offset in bits of 'tag', indexed by 'tag' (an enum)     #
//#                                                                            #
//#                                                                            #
//#  (state[O]<<offset[tag])                                                   #
//#     |     |    |                                                           #
//#     |     |    +-----------------------------+                             #
//#     +-----|------------+                     |                             #
//#           |         ___|___           _______|________                     #
//#           |   Shift state[O] right by offset[tag] bits.                    #
//#           |   ``|``          ``|``                                         #
//#           +-----+--------------+                                           #
//#                                                                            #
//##############################################################################
//}}}1
void set_sw(struct sw_t *sw, uint32_t key, int n, int s)
{
        sw->tree->peek = rb_retreive(sw->tree->root, key);
        if (sw->tree->peek == NULL) return;

        sw->tree->peek->data &= scrub[n];
        sw->tree->peek->data |= (state[s]<<offset[n]);
}
int get_sw(struct sw_t *sw, uint32_t key, int n)
{
        sw->tree->peek = rb_retreive(sw->tree->root, key);
        if (sw->tree->peek == NULL) return;
        
        sw->c = sw->tree->peek->data; // Make a copy
        sw->c &= ~(scrub[n]);         // Wipe all but nibble n
        sw->c >>= offset[n];          // Move nibble n to the end

        return (int)(sw->c);
}
/*
  Tests nibble 'n' of cell with key 'z'. If nibble has state 's', is_cell() 
  returns 1, else it returns 0.
*/
int is_sw(struct sw_t *sw, uint32_t key, int n, int s)
{
        sw->tree->peek = rb_retreive(sw->tree->root, key);
        if (sw->tree->peek == NULL) return (0);

        sw->c = sw->tree->peek->data; // Make a copy of the data
        sw->c &= ~(scrub[n]);         // Wipe all but the nibble we want
        sw->c >>= offset[n];          // Push our nibble to the end

        return (sw->c == state[s]) ? 1 : 0;
}
/*
  Returns a pointer to a string, which represents the enum tag corresponding
  to the state of nibble 'n' of cell with key 'z'.
*/
const char *get_swtag(struct sw_t *sw, uint32_t key, int n)
{
        sw->tree->peek = rb_retreive(sw->tree->root, key);
        if (sw->tree->peek == NULL) return (0);

        sw->c = sw->tree->peek->data; // Make a copy of the data
        sw->c &= ~(scrub[n]);         // Wipe all but the nibble we want
        sw->c >>= offset[n];          // Push our nibble to the end

        return sw->opt[n][sw->c];
}
/*
  Prints a formatted string consisting of the enum tags for every nibble and
  state of cell with key 'z'.
*/
void stat_sw(WINDOW *win, struct sw_t *sw, uint32_t key)
{
        int n = NNIBS-1;   // Number of nibbles in a cell (-1 for fencepost)

        sw->tree->peek = rb_retreive(sw->tree->root, key);
        if (sw->tree->peek == NULL) return;

        do {
                sw->c = sw->tree->peek->data;      // Make a copy
                sw->c &= ~(scrub[n]); // Wipe all but the nibble we want
                sw->c >>= offset[n];  // Push our nibble to the end
                wprintw(win, "%3s: %3s | ", sw->nib[n], sw->opt[n][sw->c]);
        } while (n-->0);
}
