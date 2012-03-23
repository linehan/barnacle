#include "../lib/textutils.h"

#include "gloss.h"



////////////////////////////////////////////////////////////////////////////////
void destroy_gloss(void *self)
{
       struct gloss_t *gloss = (struct gloss_t *)self; 

       free(gloss->wcs);
       free(gloss);
}



////////////////////////////////////////////////////////////////////////////////

/*
 * Returns a pointer to a newly-allocated struct gloss_t, including
 * various pretty-print methods and a destructor method.
 */
struct gloss_t *new_gloss(WINDOW *win, wchar_t *wcs, short co, short hi)
{
        struct gloss_t *new = malloc(sizeof(struct gloss_t));

        new->win = win;
        new->wcs = wcdup(wcs);
        new->len = wcslen(new->wcs);

        new->co     = co;
        new->hi     = hi;

        new->put[REVEAL]   = reveal;
        new->put[UNREVEAL] = unreveal;
        new->put[SHINE]    = shine;
        new->put[PUSH_R]   = pushr;
        new->put[PUSH_L]   = pushl;
        new->put[DROPOUT]  = dropout;

        new->die           = &destroy_gloss;

        return (new);
}
/*
 * If you have a 7-bit ASCII char * string, this function will properly
 * convert it into a wide-character sequence suitable for storage in
 * the struct gloss_t.
 */
struct gloss_t *str_gloss(WINDOW *win, const char *str, short co, short hi)
{
        wchar_t *wcs = mbdup(str);

        return wcs ? new_gloss(win, wcs, co, hi) : NULL;
}


struct glossbox_t {
        wchar_t *wcs; 
        short co;
        short hi;
};

#define __GOLD  PUR_METALYELLOW, PUR_YELLOW
#define __SILV  PUR_WHITE, PUR_SKY
#define __WARN  PUR_WHITE, PUR_RED

#define __HULK  PUR_SKY, PUR_LGREEN
#define __CERU  PUR_SKY, PUR_CERULEAN


#define NTEMPL 14
static struct glossbox_t to_pc[]={
        {L"Be advised!"       , __WARN } ,
        {L"Be alert!"         , __WARN } ,
        {L"Be aware!"         , __WARN } ,
        {L"Be calm!"          , __WARN } ,
        {L"Be careful!"       , __WARN } ,
        {L"Be on your guard!" , __WARN } ,
        {L"Be on your toes!"  , __WARN } ,
        {L"Be quiet!"         , __WARN } ,
        {L"Be still!"         , __WARN } ,
        {L"Be vigilant!"      , __WARN } ,
        {L"Be warned!"        , __WARN } ,
        {L"Be wary!"          , __WARN } ,
        {L"Be more careful!"  , __WARN } 
};


static struct glossbox_t warnings[]={ 
        {L"Alarm!"             , __WARN } ,
        {L"Arise!"             , __WARN } ,
        {L"Awake!"             , __WARN } ,
        {L"Cancel!"            , __WARN } ,
        {L"Cancelled!"         , __WARN } ,
        {L"Combat!"            , __WARN } ,
        {L"Careful!"           , __WARN } ,
        {L"Critical!"          , __WARN } ,
        {L"Critial hit!"       , __WARN } ,
        {L"Critial moment!"    , __WARN } ,
        {L"Cry havoc!"         , __WARN } ,
        {L"Danger!"            , __WARN } ,
        {L"Dangerous moments!" , __WARN } ,
        {L"Deflect!"           , __WARN } ,
        {L"En garde!"          , __WARN } ,
        {L"Interrupt!"         , __WARN } ,
        {L"Interruptus!"       , __WARN } ,
        {L"NULL!"              , __WARN } ,
        {L"On your guard!"     , __WARN } ,
        {L"Parry!"             , __WARN } 
};


static struct glossbox_t rnd[]={ 
        {L"δερποσ"          , __HULK                                   } ,
        {L"My eye!"         , __WARN                                   } ,
        {L"Smack you!"      , PUR_YEL                     , PUR_BRZ    } ,
        {L"Porkchop!"       , PUR_BROWN                   , PUR_BRZ    } ,
        {L"Beast man!"      , __HULK                                   } ,
        {L"A gentle touch!" , PUR_PURPLE                  , PUR_SKY    } ,
        {L"Tasty!"          , PUR_LRED                    , PUR_LRED   } ,
        {L"Eureka!"         , __SILV                                   } ,
        {L"Jemmy legs!"     , PUR_GREY                    , PUR_SKY    } ,
        {L"With a rope!"    , PUR_BROWN                   , PUR_WOOD   } ,
        {L"Have at you!"    , PUR_PURPLE                  , PUR_GREY   } ,
        {L"Santiago!"       , __GOLD                                   } ,
        {L"Belches!"        , PUR_LGREEN                  , PUR_SKY    } ,
        {L"My lewt!"        , PUR_LRED                    , PUR_BRZ    } ,
        {L"Sticky buns!"    , PUR_BEIGE                   , PUR_YELLOW } ,
        {L"Sink you!"       , PUR_CERULEAN                , PUR_SKY    }
};


struct gloss_t *rand_gloss(WINDOW *win)
{
        #include "../gen/dice.h"
        int i = roll_fair(NTEMPL);

        return (new_gloss(win, rnd[i].wcs, rnd[i].co, rnd[i].hi));
}
