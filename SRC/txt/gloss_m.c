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
struct gloss_t *new_gloss(WINDOW *win, const char *str, wchar_t *wcs, int colorfam)
{
        struct gloss_t *new = malloc(sizeof(struct gloss_t));

        new->win = win;

        if (str!=NULL) new->wcs = mbdup(str);
        else           new->wcs = wcdup(wcs);

        new->len = wcslen(new->wcs);

        new->co  = colorfamily[colorfam].co;
        new->hi  = colorfamily[colorfam].hi;
        new->hue[0] = colorfamily[colorfam].hue[0];
        new->hue[1] = colorfamily[colorfam].hue[1];

        new->put[REVEAL]   = reveal;
        new->put[UNREVEAL] = unreveal;
        new->put[SHINE]    = shine;
        new->put[PUSH_R]   = pushr;
        new->put[PUSH_L]   = pushl;

        new->die           = &destroy_gloss;

        return (new);
}


struct gloss_templ {
        const char *str; 
        wchar_t *wcs; 
        int cofam;
};


/*static gloss_templ msgs[]={*/
        /*{"Cancelled!"      , NULL , GLOSS_RED } ,*/
        /*{"Smack you!"      , NULL , YEL_SHINY } ,*/
        /*{"Porkchop!"       , NULL , BEI_SHINY } ,*/
        /*{"Beast man!"      , NULL , GLOSS_EEN } ,*/
        /*{"Combat!"         , NULL , PUR_GRE     , PUR_GRE} ,*/
        /*{"A gentle touch!" , NULL , PUR_GRE     , PUR_GRE} ,*/
        /*{"Tasty!"          , NULL , BEI_SHINY } ,*/
        /*{"Eureka!"         , NULL , BLU_SHINY } ,*/
        /*{"Careful!"        , NULL , GLOSS_RED } ,*/
        /*{"My eye!"         , NULL , GLOSS_PIN } ,*/
        /*{"Jemmy legs!"     , NULL , GRE_SHINY } ,*/
        /*{"With a rope!"    , NULL , WOO_SHINY } ,*/
        /*{"Have at you!"    , NULL , GRE_SHINY } ,*/
        /*{"Santiago!"       , NULL , YEL_SHINY } ,*/
        /*{"Belches!"        , NULL , GLOSS_EEN } ,*/
        /*{"My lewt!"        , NULL , GLOSS_PIN } ,*/
        /*{"Sticky buns!"    , NULL , YEL_SHINY } ,*/
        /*{"Sink you!"       , NULL , BLU_SHINY }*/
/*};*/



