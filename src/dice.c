// vim:fdm=marker
/* ==========================================================================
   FILENAME:  dice.c                                                   
   Subroutines for the generation of pseudorandom numbers, dice rolls, and
   other probabilistic results. 
   --------------------------------------------------------------------------
   DETAILS                                                               {{{1

    This document and certain functions here use the "dice notation" 
    first alluded to in the 1974 "Blue Box" edition of Dungeons & Dragons:

       "...In some places the reader will note an abbreviated notation 
        for the type of die has [sic] been used. The first number is 
        the number of dice used, the letter "d" appears, and the last 
        number is the type of dice used. Thus, "2d4" would mean that 
        two 4-sided dice would be thrown (or one 4-sided die would be 
        thrown twice); "3d12" would indicate that three 12-sided dice 
        are used, and so on." 

    The system is very simple and goes a long way toward clarifying any
    discussion that involves the rolling of dice. The convention is:

          AdX, where A: the number of dice
                     X: the number of sides ("faces") on each die

    This notation also accomodates various mathematical operators that can
    be used to shape the outcome of a roll or series of rolls:

          AdX+B, where B: a number added to the sum of AdX

    Other operators that can be used include -, *, %, or anything else you
    put your mind to. Along with these are the special operands L (lowest)
    and H (highest), which take the lowest or highest result of the rolls 
    as their value, e.g.

          AdX-L (Roll AdX and discard the lowest result)

    It can be seen that for this example, the probability curve of the rolls 
    will be skewed toward the higher numbers. When rolling dice, we want to 
    ensure that the results are normally distributed. To do this, consider 
    the central limit theorem:

    Let ⦃X₁,...,Xₙ⦄ be a random sample of size n -- that is, a sequence of 
    independent and identically distributed random variables with expected 
    values μ and variances σ². 
       
    Suppose we are interested in the behavior of the sample average of 
    these random variables: Sₙ := (X₁+⋯+Xₙ)/n. 
       
    The central limit theorem asserts that as n gets larger and larger, 
    the distribution of Sₙ approximates normal, with mean μ and variance 
    (1/n)σ².
       
    In fact, Sₙ approaches a normal distribution regardless of the shape 
    of the distributions of individual X's!

    So in order to make the results of our dice rolls normally distributed, 
    it is always a good idea to roll at least three dice, and work on the 
    aggregate of these throws.                                           }}}1
   --------------------------------------------------------------------------
   ========================================================================== */
   #include <stdio.h>
   #include <stdarg.h>
   #include <stdlib.h>
   #include <string.h>
   #include <time.h>
   #include <math.h>

   #include "lib/mtwist.h"
   #include "lib/list/list.h"

   #define HEADS 1  
   #define TAILS 0 

   typedef struct dice_t {
     int n; /* number of rolls */
     int d; /* number of sides */
     double *w; /* weight of each side */
   } DICE;

   int MT_PRIMED = 1;
   const double ONE = 1.0;
   const double TWO = 2.0;
/* ========================================================================== */
/* --------------------------------------------------------------------------
   Initialize rand() and mt_random()
   -------------------------------------------------------------------------- */
   int init_random()
   {
     /* Seed rand() with the system time BEFORE calling mt_init, otherwise
      * all your numbers will start looking awfully familiar... */
     srand(time(NULL));
     mt_init();
     MT_PRIMED = 0;

     return 0;
   }
/* --------------------------------------------------------------------------
   Transform the return value of mt_random() to the range [0, 1].
   -------------------------------------------------------------------------- */
   double mt_normalize()
   {
       double max = (double)MT_MAX;
       double a   = (double)mt_random();

       return (a/max);
   }
/* --------------------------------------------------------------------------
   Flip a biased coin with probability of heads = b(ias) 
   
   Generates a random value between 0 and 1. If b (<1) is less than the
   random value generated, the coin is said to be "heads", otherwise it
   is "tails".
   -------------------------------------------------------------------------- */
   int flip_biased(double b)
   {
     return (mt_normalize()<b) ? 1 : 0;
   }
/* --------------------------------------------------------------------------
   Return a pointer to a die
   -------------------------------------------------------------------------- */
   int roll_fair(int n)                                                          
   {                                                                             
     int i;                                                                      
     for (i=0; i<n; i++)                                                         
     {                                                                           
       if (flip_biased((ONE / ((double)(n-i)))) == HEADS) return i;              
     }                                                                           
     return (n+1); /* Impossible */                                              
   }                                   
/* --------------------------------------------------------------------------
   Return a pointer to a die
   -------------------------------------------------------------------------- */
   DICE *new_dice(char *str, ...)
   {
     int i;
     DICE *new = (DICE *)malloc(sizeof(DICE));

     sscanf(str, "%dd%d", &(new->n), &(new->d));
     new->w = (double *)malloc((new->d) * sizeof(double));

     va_list(argp); 
     va_start(argp, str);
     for (i=0; i<(new->d); i++) {
       new->w[i] = va_arg(argp, double);
     }
     va_end(argp);

     return new;
   }
/* --------------------------------------------------------------------------
   Implement Vose's alias method
   -------------------------------------------------------------------------- */
   int vose_alias(DICE *D)
   {
     int d = D->d; /* number of sides */

     double ONE = 1.0; /* Makes life convenient later on */
     double *Pr[d];    /* Adjusted probability values */
     double *Al[d];    /* References to certain adjusted probability values */
     double   p[d];    /* A copy of the provided probability array _p[n] */
     int i, S;

     struct wrk {
       double *pptr;
       struct list_node node;
     } *tmp_l, *tmp_s;

     LIST_HEAD(Sm); /* Worklist heads */
     LIST_HEAD(Lg);

     for (i=0; i<d; i++) 
     {
         Pr[i] = NULL;
         Al[i] = NULL;
         struct wrk *new = (struct wrk *)malloc(sizeof(struct wrk));
         p[i] = (D->w[i] * (double)D->d);
         new->pptr = &(p[i]);
         if (p[i] < ONE) list_add(&Sm, &(new->node));
         else            list_add(&Lg, &(new->node));
     }

     i=0; /* Since loop entry is indeterminate */
     for (; ((!(list_empty(&Sm))) && (!(list_empty(&Lg)))); i++) 
     {
         tmp_s = list_top(&Sm, struct wrk, node);
                 list_del_from(&Sm, &(tmp_s->node));
         tmp_l = list_top(&Lg, struct wrk, node);
                 list_del_from(&Lg, &(tmp_l->node));

         Pr[i] = tmp_s->pptr;
         Al[i] = tmp_l->pptr;

         *(Al[i]) = (( *(Al[i]) + *(Pr[i]) ) - ONE );

         if (*(Al[i]) < ONE) list_add(&Sm, &(tmp_l->node));
         else                list_add(&Lg, &(tmp_l->node));
     }
     for (; (!(list_empty(&Lg))); i++) /* LARGE not empty */
     {
         Pr[i] = &ONE;
         tmp_l = list_top(&Lg, struct wrk, node);
                 list_del_from(&Lg, &(tmp_l->node));
                 free(tmp_l);
     }
     for (; (!(list_empty(&Sm))); i++) /* SMALL not empty */
     {
         Pr[i] = &ONE;
         tmp_s = list_top(&Sm, struct wrk, node);
                 list_del_from(&Sm, &(tmp_s->node));
                 free(tmp_s);
     }
     /*printf(" i = %d\n", i);*/
     /*printf(" Prob\tAlias\n");*/
     /*for (i=0; i<n; i++) {*/
         /*if (Pr[i] == NULL) printf(" NULL\t");*/
         /*else               printf(" %g\t", *(Pr[i]));*/
         /*if (Al[i] == NULL) printf("NULL\n");*/
         /*else               printf("%g\n", *(Al[i]));*/
     /*}*/
     for (i=0; i<d; i++)
     {
         S = roll_fair(d);
         if (flip_biased(*(Pr[S])) == HEADS) return S;
         else                              return (int)*(Al[S]);
     }
     return (d+1); /* Impossible */
   }

