#ifndef __PLAYER_CHARACTER
#define __PLAYER_CHARACTER

#include <string.h>
#include "../eng/state.h"

/* Holds all player characters in the program. */
struct rb_tree *PC_TREE;
#define MAX_PEOPLE 100
uint32_t person_list[MAX_PEOPLE];




/*
  A character's stats are the classic D&D-style modifiers that determine ability and
  situational performance. 
*/
#define NATTRS 8
static const char *attr_tag[]={"STR","DEX","VIT","AGI","INT","WIS","CHA","LUC"};
enum attr_enum { STR,  DEX,  VIT,  AGI,  INT,  WIS,  CHA,  LUC };
static const wchar_t attr_sym[]={L"ΣΦΔAΨWΧΛ"};
static const wchar_t attr_str[]={L"Σ   Φ   Δ   A   Ψ   W   Χ   Λ"};


#define TAG(n) attr_tags[n]
#define SYM(n) attr_symb[n]



#define MAX_BUFFER 30



struct character_t {
        char fname[MAX_BUFFER];
        char mname[MAX_BUFFER];
        char lname[MAX_BUFFER];
        char birth[MAX_BUFFER];
        uint8_t job;
        uint8_t age;
        uint8_t weight;
        uint8_t height;
        uint8_t gender;
        uint32_t vitals;
};



#define JOB_COUNT 51
/* These are the professions a character can have. */
static const char *job_list[] = { 
        "Able Seaman",
        "Accountant",
        "Actor",
        "Aristocrat",
        "Astronomer",
        "Barber",
        "Beggar",
        "Blacksmith",
        "Bookkeeper",
        "Botanist",
        "Butcher",
        "Carpenter",
        "Chemist",
        "Cleric",
        "Cooper",
        "Diver",
        "Driver",
        "Engineer",
        "Farmer",
        "Grocer",
        "Highwayman",
        "Horologist",
        "Innkeeper",
        "Inventor",
        "Linguist",
        "Longshoreman",
        "Monk",
        "Musician",
        "Navigator",
        "Army Officer",
        "Navy Officer",
        "Optician",
        "Painter",
        "Philosopher",
        "Physician",
        "Pilot",
        "Planter",
        "Poet",
        "Prostitute",
        "Schoolmaster",
        "Scribe",
        "Servant",
        "Shepherd",
        "Shopkeeper",
        "Slave",
        "Soldier",
        "Surgeon",
        "Surveyor",
        "Tailor",
        "Thief",
        "Weaver",
        (char *)NULL 
};
/* Flavor text to be printed as a sort of "job description". */
static const char *job_flavor[] = {
        "You know how to handle a boat.",
        "Beans can't count themselves.",
        "This could be your big breakthrough.",
        "Armchair adventures are so tedious.",
        "Save your batteries, the GPS;          my tables guarantee success.",
        "You trim, you shave, you even root canal.",
        "You're a hobo hero!",
        "And he sang: \"Hurra for my handiwork!\" And the red sparks lit the air;",
        /* Charles Mackay, Tubal Cain, Stanza 4. */
        "You--SHHH!",
        "You've always been excited about binominal nomenclature.",
        "Your cold cuts are first-rate.",
        "You are the most valuable thing on a ship.",
        "Double, double toil and trouble;     Fire burn, and caldron bubble.",
        /* William Shakespeare, Macbeth (c. 1605) Act IV, scene 1 */
        "Whose faith has centre everywhere,   Nor cares to fix itself to form.",
        /* Alfred Tennyson, In Memoriam A.H.H. (1849), XXXIII. */
        "Your barrels are only matched by your buckets.",
        "Water is a very good servant, but it is a cruell maister.",
        /* William Bullein in Bulwarke of Defence against All Sicknesses (1562) */
        "Spare the rod, the horse, the buggy.",
        "An invincible wall, its only weakness.",
        "Enjoy the cows, the cats, the soiled clothes, the floppy hats.",
        "You feed the people, for a price.",
        "There is no safety this side of the grave.",
        /* Robert Heinlein, Stranger in a Strange Land (1961) */
        "The whirligig of time brings in your revenges.",
        /* William Shakespeare, Twelfth Night (c. 1601-02), Act V, scene 1, line 384. (sub. "his") */
        "You know how to make a bed look comfortable.",
        "One spanner, one hour, one bag of cheetos.",
        "Vi estas adepto en antikvaj esperanto.", 
        /* (trans.) "You are adept at ancient esperanto." */
        "More tattoo than man.", 
        "You do a lot of thinking.",
        "You're poor in wealth, sometimes spoony.",
        "You've got maps.",
        "You're a dough boy.",
        "You're a sardine.",
        "\"Open wide...\"",
        "Your eyes are the beholder.",
        "What is \"you\", really?",
        "You know the leg-bones, ankle-bones, all those bones.",
        "What's under the water?",
        "Grow'd good ol' southurn tobacca.", 
        "Poetry's a mere drug, Sir.",
        /* George Farquhar, Love and a Bottle (1698) */
        "Pleasure in others.",
        "Raise your hand before speaking.",
        "Word processor.",
        "You live to serve.",
        "Sheep are people, too.",
        "You've studied Taloon.",
        "To serve the user.",
        "Your fist is ever ready for a knock-down blow.",
        "\"Hold still\"!",
        "This theodolite cost more than your house.",
        "I can sew, so what?",
        "You can't help yourself.",
        "You weave stuff.",
        (char *)NULL
};



static struct character_t *focused;


static inline void focus(uint32_t key)
{
        focused = (struct character_t *)rb_extra(PC_TREE, key);
}



/* Return character stat n */
static inline uint32_t get_cstat(uint32_t key, int n)
{
        return (get_state(PC_TREE, key, 0, n));
}
/* Set character stat n to value v */
static inline void set_cstat(uint32_t key, int n, int v)
{
        set_state(PC_TREE, key, 0, n, v);
}
/* Increment character stat n by 1 */
static inline void inc_cstat(uint32_t key, int n)
{
        set_state(PC_TREE, key, 0, n, get_cstat(key, n)+1);
}
/* Decrement character stat n by 1 */
static inline void dec_cstat(uint32_t key, int n)
{
        set_state(PC_TREE, key, 0, n, get_cstat(key, n)-1);
}
/* Returns job name. */
static inline char *getjob(uint32_t key)
{
        focus(key);
        return (job_list[(int)focused->job]);
}
/* Returns first name. */
static inline char *fname(uint32_t key)
{
        focus(key);
        return (focused->fname);
}
/* Returns middle name. */
static inline char *mname(uint32_t key)
{
        focus(key);
        return (focused->mname);
}
/* Returns last name. */
static inline char *lname(uint32_t key)
{
        focus(key);
        return (focused->lname);
}
/* Returns full name (just a tweaked three_name_hash). */
static inline char *flname(uint32_t key)
{
        char *f, *l;

        f = fname(key);
        l = lname(key);

        char *fl = malloc(strlen(f) + strlen(l) + 2);

        sprintf(fl, "%s %s", f, l);

        return (fl);
}
/* Returns full name (just a tweaked three_name_hash). */
static inline char *fullname(uint32_t key)
{
        char *a, *b, *c;

        a = fname(key);
        b = mname(key);
        c = lname(key);

        char *abc = malloc(strlen(a) + strlen(b) + strlen(c) + 3);

        sprintf(abc, "%s %s %s", a, b, c);

        return (abc);
}


static inline int npersons(void)
{
        if (PC_TREE == NULL)    return 0;
        else                    return (PC_TREE->n);
}

static inline uint32_t *vit(uint32_t key)
{
        focus(key);
        return (&focused->vitals);
}

void load_pc_test(void);
void attr_see(wchar_t *wch, uint32_t key);

uint32_t new_pc(char *firname,
                char *midname,
                char *lasname,
                char *birthpl,
                uint8_t job,
                uint8_t age,
                uint8_t weight,
                uint8_t height,
                uint8_t gender);

#endif
