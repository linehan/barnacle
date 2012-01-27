// vim:fdm=marker
/*******************************************************************************
 FILENAME: psh.c
 The sound the reader would make if it stood for anything except pygmy shell.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#include "../lib/llist/list.h"
#include "../lib/hash/hash.h"
#include "../gen/dice.h"
#include "../gen/perlin.h"
/******************************************************************************/
#define MAX_PHRASE_LEN 500
#define MAX_PHRASE_NUM 10
/* Prepare the ncurses window for text entry */
#define PROMPT_ON(win)  \
        keypad(win, 1); \
        echo();         \
/* Print the prompt character on the desired window */
#define PROMPT(win) \
        mvwprintw(win, 1, 0, " > ")
/* Erase WINDOW win and re-print the prompt string */
#define REPROMPT(win) \
        werase(win);  \
        PROMPT(win)
/* Return the ncurses window to its previous state. */
#define PROMPT_OFF(win) \
        REPROMPT(win);  \
        keypad(win, 0); \
        noecho()
/* Add a PHRASENODE or a WORDNODE to their respective lists heads */
#define ADD_PHRASE(ph, str) \
        list_add(str->phraselist, &(ph->node))
#define ADD_WORD(w, ph) \
        list_add(ph->wordlist, &(w->node))
/* Allocate, check, and initialize a struct list_head pointer */
#define LIST_HEAD_NEW(head)                                          \
        head = (struct list_head *)malloc(sizeof(struct list_head)); \
        list_head_init(head)                                         \
/* Returns 0 (FALSE) if the buffer is blank, i.e., consists of a newline only */
#define NOT_BLANK(buffer) \
        buffer[0] != '\0' ? 1 : 0

/* The main input buffer is allocated on the heap */ 
static char BUFFER[MAX_PHRASE_LEN];
/******************************************************************************
 * OVERVIEW -- How psh decomposes user input 
 *
 * There are two components to a given input string, each represented by its
 * own datatype. The larger is the PHRASE structure, which is a node in a
 * linked list of PHRASEs. A PHRASE contains a given terminator-delimited 
 * string. Acceptable terminators are:
 *
 *      . period
 *      ! exclamation mark
 *      ? question mark
 *      ; semicolon
 *      : colon
 *
 * Each PHRASE structure contains the head of another linked list, 'words', 
 * which contains 'l' nodes of type WORD, which contain a word string and
 * the length of that string. 
 *
 * A final illustration:
 *
 *      Gil-Galad was an Elven-king. }------------- PHRASE
 *      Of him the harpers sadly sing: }----------- PHRASE
 *      The last whose realm was fair and free 
 *      Between the mountains and the sea. }------- PHRASE        
 *
 *      But long ago he rode away,
 *      And where he dwelleth none can say. }------ PHRASE
 *      For into darkness fell his star; }--------- PHRASE
 *      In Mordor, where the shadows are." }------- PHRASE
 *
 *      Gil-Galad was an Elven-king.
 *      --------- --- -- ---------- \
 *         WORD     |   \    WORD    Terminator character is added  
 *                WORD  WORD         to the PHRASE structure for reference.
 *
 ******************************************************************************/
/******************************************************************************
 * The WORD structure is a simple node in a linked list of WORDs, which holds
 * a null-terminated string in 'word', (the word) and an integer 'l',
 * initialized to the length of the string at 'word'.
 ******************************************************************************/
typedef struct word_node {
        char *word;
        int len;
        struct list_node node;
} WORD;
/******************************************************************************
 * The PHRASE structure looks complicated, but is designed for convenience.
 *
 * There are three int values, and these are simple. 'tp' is the terminal
 * punctuation mark of the phrase string, 'n' is the number of WORDS in the
 * PHRASE, and 'l' is the length of the entire phrase string, including
 * whitespace.
 *
 * The 'node' entry is, of course, a structure which can associate the PHRASE
 * with a linked list of other PHRASEs, using the Linux Kernel's linked list
 * API in lib/llist/list.h.
 *
 * There is an "active word" pointer 'w', which points to a WORD in the linked 
 * list anchored at 'wordlist'. The WORD which 'w' is pointing to can be 
 * shifted in either direction using the pseudo-methods nextword() and 
 * prevword(), which will set the pointer at 'w' to the next or previous WORD 
 * in the list, relative to the current WORD at 'w'.
 *
 *****************************************************************************/
typedef struct phrase_node {
        char *phrase;
        int tp; /* terminal punctuation mark */
        int n;  /* number of words in phrase string */
        int len;  /* length of phrase string, including whitespace */
        WORD *w; /* the currently active word */
        struct list_head *wordlist;
        struct list_node node;
        int (*nextword)(const void *self);
        int (*prevword)(const void *self);
} PHRASE;
/******************************************************************************
 * Finally, there is the STRING structure, which is a container to house the
 * head of a linked list of PHRASE nodes, as well as a couple of methods, very
 * similar to the ones already documented for PHRASEs, above.
 * 
 * Each time a line of input is read, the final result is a STRING, with the
 * various terminal-punctuation-delimited PHRASE nodes, and, in turn, the
 * whitespace-delimited WORD nodes. This allows a finer level of access and
 * control to the manipulation and processing of user input.
 *
 * Finally, the unspool() method will safely delete all the nodes of the
 * phraselist, and each of their respective wordlists, and free()'ing any 
 * allocated structures or memory items, before calling free() on the STRING 
 * itself, effectively disappearing into its own hat!
 ******************************************************************************/
typedef struct string_node {
        int n;
        int len;
        PHRASE *p;
        struct list_head *phraselist;
        int (*nextphrase)(const void *self);
        int (*prevphrase)(const void *self);
        int (*unspool)(const void *self);
} STRING;

/******************************************************************************
 * This is the function which will be pointed to by the method 'nextword' in
 * a PHRASE structure. It accepts the phrase structure as its argument, through
 * a void pointer which must be cast to the proper type. The WORD pointed to
 * by the element 'w' of the PHRASE is then set to the top element of the
 * wordlist, and (once safely referenced at 'w'), the WORD node is removed
 * from the wordlist and re-added as the last item.
 ******************************************************************************/
int next_word(const void *phrase)
{
        /* Cast the void pointer */
        PHRASE *ph = (PHRASE *)phrase;

        if (list_empty(ph->wordlist))
                return 0;
        else {
                ph->w = list_top(ph->wordlist, WORD, node);
                        list_del_from(ph->wordlist, &ph->w->node);
                        list_add_tail(ph->wordlist, &ph->w->node);
        }
        return 1;
}
int next_phrase(const void *string)
{
        STRING *str = (STRING *)string;

        if (list_empty(str->phraselist))
                return 0;
        else {
                str->p = list_top(str->phraselist, PHRASE, node);
                         list_del_from(str->phraselist, &str->p->node);
                         list_add_tail(str->phraselist, &str->p->node);
        }
        return 1;
}
/******************************************************************************
 * Pointed to by the method 'prevword' in a PHRASE structure. Its operation 
 * is obviously very similar to next_word, except it points 'w' to the last
 * node of the wordlist, instead of the first, and subsequently removes this
 * last node and places it at the top of the wordlist.
 ******************************************************************************/
int prev_word(const void *phrase)
{
        /* Cast the void pointer */
        PHRASE *ph = (PHRASE *)phrase;

        if (list_empty(ph->wordlist))
                return 0;
        else {
                ph->w = list_tail(ph->wordlist, WORD, node);
                        list_del_from(ph->wordlist, &ph->w->node);
                        list_add(ph->wordlist, &ph->w->node);
        }
        return 1;
}
int prev_phrase(const void *string)
{
        /* Cast the void pointer */
        STRING *str = (STRING *)string;

        if (list_empty(str->phraselist))
                return 0;
        else {
                str->p = list_tail(str->phraselist, PHRASE, node);
                         list_del_from(str->phraselist, &str->p->node);
                         list_add(str->phraselist, &str->p->node);
        }
        return 1;
}
/******************************************************************************
 * Pointed to by the method 'unspool' in a PHRASE structure. It uses the
 * function list_for_each_safe(), defined in lib/llist/list.h, to iterate
 * over the items in the wordlist, removing each from the list before 
 * free()'ing it. Once this is accomplished, list_empty() confirms the deed
 * is done, the PHRASE itself is free()'d.
 *
 * IMPORTANT: If your PHRASE is part of a linked list, it must first be
 *            removed from that list using list_del_from(), before using
 *            this method. Otherwise, your list will have a wormhole to 
 *            the wild side where this node used to be!
 ******************************************************************************/
int unspool_string(const void *string)
{
        /* Cast the void pointer */
        STRING *str = (STRING *)string;

        /* Used by list_for_each_safe() during iteration */
        PHRASE *nextp, *tmpp;
          WORD *nextw, *tmpw;

        list_for_each_safe(str->phraselist, tmpp, nextp, node) {
                /* Unspool and free each WORD in the current wordlist */
                list_for_each_safe(tmpp->wordlist, tmpw, nextw, node) {
                        list_del_from(tmpp->wordlist, &tmpw->node);
                        free(tmpw->word);
                        free(tmpw);
                }
                /* Unspool and free each PHRASE in the phraselist */
                list_del_from(str->phraselist, &tmpp->node);
                if (tmpp->wordlist != NULL)
                        free(tmpp->wordlist);
                if (tmpp != NULL)
                        free(tmpp);
        }
        /* Free the STRING structure */
        if (str->phraselist != NULL) 
                free(str->phraselist);
        if (str != NULL)
                free(str);
        return 1;
}
/******************************************************************************
 * Accepts an input string and initializes a new PHRASE structure to house
 * it. It is fairly straightforward, read the in-line documentation.
 ******************************************************************************/
PHRASE *new_phrase(char *input)
{
        PHRASE *ph = malloc(sizeof(PHRASE));

        ph->n = 0;
        ph->len = strlen(input);
        ph->phrase = malloc(ph->len * sizeof(char));
        strcpy(ph->phrase, input);
        ph->w = NULL;
        ph->tp = input[ph->len-1]; /* note the terminal (ending) punctuation */

        /* Allocate and initialize wordlist head */
        LIST_HEAD_NEW(ph->wordlist);

        /* Attach the methods to the function pointers in PHRASE. */
        ph->nextword = &next_word;
        ph->prevword = &prev_word;
        
        /* Split the phrase into a set of whitespace-delimited tokens,
         * i.e. words. Give each word its own WORD node, and spool it
         * on to the wordlist. */
        char *word, *save;

        for (word = strtok_r(input, " ", &save);
             word;
             word = strtok_r(NULL, " ", &save))
        {
                WORD *new = malloc(sizeof(WORD));
                            new->len = strlen(word);
                            new->word = malloc(new->len * sizeof(char));
                            strcpy(new->word, word);
                ADD_WORD(new, ph);
                ph->n =+ 1; /* Increment the number of words in the PHRASE */
        }
        return ph;
}
/******************************************************************************
 * Initializes a new STRING type.
 ******************************************************************************/
STRING *new_string(void)
{
        STRING *str = malloc(sizeof(STRING));

        str->n = 0;
        str->len = 0;
        str->p = NULL;

        /* Allocate and initialize phraselist head */
        LIST_HEAD_NEW(str->phraselist);

        /* Assign methods */
        str->nextphrase = &next_phrase;
        str->prevphrase = &prev_phrase;
        str->unspool    = &unspool_string;

        return str;
}

/******************************************************************************
 * Opens a command prompt to receive user input, optionally sending it to
 * a callback function to be processed. Each loop first gathers a "peek"
 * character to determine whether the user is entering a command or simply
 * typing, and takes some appropriate action. When a full line is entered,
 * a new PHRASE node is initialized for that line of text, and this node is
 * added to the STRING linked list.
 ******************************************************************************/
int wlisten(WINDOW *win, void (*callback)(void *input))
{
        short peek;
        STRING *str = new_string();

        PROMPT_ON(win);
        PROMPT(win);
        while ((peek = wgetch(win)) != '`') {
                if ((peek == KEY_UP)) {
                       /* As long as the user keeps pressing KEY_UP,
                        * don't call ungetch(), but cycle backwards
                        * through the phraselist and print each phrase
                        * on the prompt. */
                        do {    REPROMPT(win);
                                if (str->nextphrase(str))
                                        wprintw(win, "%s ", str->p->phrase);
                        } while ((peek = wgetch(win)) == KEY_UP);
                       /* Since the last peek was the one which broke the
                        * do loop, it follows that it's probably actual
                        * input that belongs in the input stream, so that's
                        * where we send it. */
                        ungetch(peek);
                       /* Next, we feed the phrase we've selected back into
                        * the input stream as well. This is an important
                        * point -- although we printed this phrase in the
                        * do loop above, the line buffer cannot see it. The
                        * line buffer consists entirely of characters from
                        * an input stream opened during a call to wgetch()
                        * or wgetstr(), et al.
                        *
                        * The function ungetch() places its argument in the 
                        * input stream, and there it sits until another 
                        * function like wgetch() opens the input stream and 
                        * captures it, along with any other chars that have 
                        * accumulated, as though the user had typed them in 
                        * almost instantaneously. */
                        if (str->p != NULL) {
                                int i = (str->p->len);
                                while (i--) ungetch(str->p->phrase[i]);
                        }
                       /* Now we can proceed normally. */
                }
                else ungetch(peek);
                REPROMPT(win);
               /* Open the input string and receive a string of length
                * MAX_PHRASE_LEN in the buffer. Because the function
                * wants to get a *string*, it will loiter at the input
                * stream until it receives a newline. */
                wgetnstr(win, BUFFER, MAX_PHRASE_LEN);
               /* As long as the buffer (the line) wasn't empty, create
                * a new PHRASE node for it and send the node to the
                * processing function. Once the processing function returns
                * successfully, go ahead and add the node to the phraselist. */
                if (BUFFER[0] != '\0') {
                        PHRASE *ph = new_phrase(BUFFER);
                        callback(ph);
                        ADD_PHRASE(ph, str);
                }
               /* Erase the prompt and loop. */
                REPROMPT(win);
        }
        PROMPT_OFF(win);
       /* Unspool the STRING we've been working with. */
        str->unspool(str);
        return 1;
}
