#define MAX_PHRASE_LEN 500
#define MAX_PHRASE_NUM 10

#define ADD_PHRASE(ph, str) list_add(str->phraselist, &(ph->node))
#define ADD_WORD(w, ph) list_add(ph->wordlist, &(w->node))

#define IFMATCH(str1, str2) if ((memcmp(str1, str2, strlen(str2)) == 0))
#define ELMATCH(str1, str2) else if ((memcmp(str1, str2, strlen(str2)) == 0))

/* Print the prompt character on the desired window */
#define PROMPT(win) mvwprintw(win, 1, 0, " > ")
/* Erase WINDOW win and re-print the prompt string */
#define REPROMPT(win) { werase(win); \
                        PROMPT(win); \
                      }
/* Prepare the ncurses window for text entry */
#define PROMPT_ON(win) { PROMPT(win);    \
                         keypad(win, 1); \
                         echo();         \
                       }
/* Return the ncurses window to its previous state. */
#define PROMPT_OFF(win) { werase(win);    \
                          keypad(win, 0); \
                          noecho();       \
                        }
/* Return a pointer to a char array precisely as large as the argument. */
#define bigas(string) (char *)malloc(strlen(string) * sizeof(char))
/* Initialize an existing pointer to an array of char to the size of the
 * argument string. */
#define fit(dest, source) dest = bigas(source);
/* Allocate memory for and copy string 'source' to the (uninitialized)
 * char pointer 'dest' */
#define dup(dest, source) { fit(dest, source); strcpy(dest, source); }

/* Returns 0 (FALSE) if the buffer is blank, i.e., consists of a newline only */
#define NOT_BLANK(buffer) buffer[0] != '\0' ? 1 : 0;

/* Strings are the horizontal component of sentences. */
typedef struct strnode {
        char *word;
        int l;
        struct list_node node;
} WORD;

/* Catenae are the vertical component of a sentence, the semantic structures. */
typedef struct phrase {
        const char *phrase;
        char terminator;
        int n;
        int l;
        WORD *w;
        struct list_head *words;
        struct list_node node;
        int (*nextword)(const void *self);
        int (*prevword)(const void *self);
} PHRASE;

typedef struct string_node {
        int n;
        int l;
        PHRASE *p;
        struct list_head *phraselist;
        int (*nextphrase)(const void *self);
        int (*prevphrase)(const void *self);
        int (*unspool)(const void *self);
} STRING;

PHRASE *new_phrase(const char *input);
STRING *new_string(void);
