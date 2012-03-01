// vim:fdm=marker
#include <stdint.h>
#include <ncurses.h>
#include <panel.h>
#include "../pan/test.h"
#include "domain.h"

//##############################################################################
// Allocate memory for a new struct dom_t, and initialize it.
//##############################################################################
struct dom_t *new_dom(TOC nibbles, GLOSS options)
{
        struct dom_t *new = malloc(sizeof *new);

        new->tree = new_tree();
        new->nib = nibbles;
        new->opt = options;
        new->c = 0;

        return (new);
}
//##############################################################################
// Retreive a cell with key 'z', and set its 'n'th nibble to state 's'.
//##############################################################################
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
void set_state(struct dom_t *dom, uint32_t key, int n, int s)
{
        dom->tree->peek = rb_retreive(dom->tree->root, key);
        if (dom->tree->peek == NULL) return;

        dom->tree->peek->data &= scrub[n];
        dom->tree->peek->data |= (state[s]<<offset[n]);
}
//##############################################################################
// Retreive the integer state of nibble 'n'
//##############################################################################
int get_state(struct dom_t *dom, uint32_t key, int n)
{
        dom->tree->peek = rb_retreive(dom->tree->root, key);
        if (dom->tree->peek == NULL) return;
        
        dom->c = dom->tree->peek->data; // Make a copy
        dom->c &= ~(scrub[n]);         // Wipe all but nibble n
        dom->c >>= offset[n];          // Move nibble n to the end

        return (int)(dom->c);
}
//##############################################################################
// Tests nibble 'n' of cell with key 'z'. If nibble has state 's', is_cell() 
// returns 1, else it returns 0.
//##############################################################################
int is_state(struct dom_t *dom, uint32_t key, int n, int s)
{
        dom->tree->peek = rb_retreive(dom->tree->root, key);
        if (dom->tree->peek == NULL) return (0);

        dom->c = dom->tree->peek->data; // Make a copy of the data
        dom->c &= ~(scrub[n]);         // Wipe all but the nibble we want
        dom->c >>= offset[n];          // Push our nibble to the end

        return (dom->c == state[s]) ? 1 : 0;
}
//##############################################################################
// Returns a pointer to a string, which represents the enum tag corresponding
// to the state of nibble 'n' of cell with key 'z'.
//##############################################################################
const char *get_state_tag(struct dom_t *dom, uint32_t key, int n)
{
        dom->tree->peek = rb_retreive(dom->tree->root, key);
        if (dom->tree->peek == NULL) return (0);

        dom->c = dom->tree->peek->data; // Make a copy of the data
        dom->c &= ~(scrub[n]);         // Wipe all but the nibble we want
        dom->c >>= offset[n];          // Push our nibble to the end

        return dom->opt[n][dom->c];
}
//##############################################################################
// Prints a formatted string consisting of the enum tags for every nibble and
// state of cell with key 'z'.
//##############################################################################
void stat_state(WINDOW *win, struct dom_t *dom, uint32_t key)
{
        int n = NNIBS-1;   // Number of nibbles in a cell (-1 for fencepost)

        dom->tree->peek = rb_retreive(dom->tree->root, key);
        if (dom->tree->peek == NULL) return;

        do {
                dom->c = dom->tree->peek->data;      // Make a copy
                dom->c &= ~(scrub[n]); // Wipe all but the nibble we want
                dom->c >>= offset[n];  // Push our nibble to the end
                wprintw(win, "%3s: %3s | ", dom->nib[n], dom->opt[n][dom->c]);
                wprintw(DIAGNOSTIC_WIN, "%3s: %3s | ", dom->nib[n], dom->opt[n][dom->c]);
        } while (n-->0);
}
