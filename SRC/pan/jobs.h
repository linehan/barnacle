#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include "../gfx/gfx.h"


//uint32_t human_attr;
//enum human_attr { 
       //STR=0,    DEX=2,     VIT=4,     AGI=6,   INT=8,        CHA=10, 
////     Strength  Dexterity  Vitality   Agility  Intelligence  Charm
       //TRA=12,   WEA=14,    EDU=16,    MAL=18,  BEN=20, 
////     Travel    Wealth     Education  Malice   Benificence
       //EAR=22,   EYE=24,    NOS=26,    TOU=28,  TAS=30 
////     Hearing   Sight      Smell      Touch    Taste
//};


const char *job_descriptions[] = {
        "You know how to handle a boat.",
        "More than a bean-counter.",
        "You were born to do this.",
        "Adventures are so tedious.",
        "Better than GPS.",
        "Trim and a shave, and also root canals.",
        "Hobo hero, ready for anything.",
        "You can handle an anvil.",
        "Shhh!",
        "You've always been excited about binominal nomenclature.",
        "Your cold cuts are first-rate.",
        "You are the most valuable thing on a ship.",
        "Adept at stink-bombs, tonics, fire-related gifts.",
        "You don't have any healing spells, but they don't need to know that.",
        "Your barrels are only matched by your buckets.",
        "How long can you hold your breath?",
        "Time to trade in your horse and buggy.",
        "You built an invincible wall and invented its only weakness.",
        "Pigs and goats and cows and cats, soiled clothes and floppy hats.",
        "People have to eat, but they don't have to farm.",
        "A stranger on the open road.",
        "The whirligig of time brings in his revenges.",
        //"Come what come may, time and the hour runs through the roughest day.",
        "You know how to make a bed look comfortable.",
        "Give me a screwdriver, two hours, and a bag of cheetos.",
        "Vi estas adepto en antikvaj esperanto.", // You are adept at ancient esperanto
        "More tattoo than man.", 
        "You do a lot of thinking.",
        "Poor in wealth and often spoony.",
        "You've got maps.",
        "Dough boy.",
        "Sardine.",
        "Open wide...",
        "Your eyes are the beholder.",
        "What is a man?",
        "Leg-bones, ankle-bones, all those bones.",
        "You know what's under the water.",
        "Good ol' southurn tobacca.", 
        "Good with words.",
        "How's it going, big-time?",
        "Raise your hand before speaking.",
        "Your hand hurts.",
        "Yes, sir.",
        "There is more to sheep than meets the eye.",
        "FIGHT",
        "Hold still.",
        "Over the hills and far away.",
        "Stand up straight.",
        "What, this? I got it from a friend.",
        "I weave stuff.",
        (char *)NULL
};

const char *job_list[] = { 
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
        "Priest",
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



#define NJOBS 54
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

MENU   *job_menu;
WINDOW *job_menu_win;
WINDOW *job_desc_win;

static void set_menu_color(MENU *menu, short pair)
{
        WINDOW *win = menu_sub(menu);
        wcolor_set(win, pair, NULL);
        vrt_refresh();
}

static inline set_menu_info(MENU *menu, int h, int w, int y, int x)
{
        WINDOW *win = menu_win(menu);
        job_desc_win = derwin(win, h, w, y, x);
}

static void post_job_info(MENU *menu)
{
        int ind = item_index(current_item(menu));
        werase(job_desc_win);
        wprintw(job_desc_win, job_descriptions[ind]);
        vrt_refresh();
}

static void put_job_menu(WINDOW *win, int y, int x)
{
        int i;
        ITEM *job_items[NJOBS];

        i = NJOBS;
        for (i=0; i<NJOBS; i++) {
                job_items[i] = new_item(job_list[i], job_list[i]);
        }
        
        job_menu = new_menu(job_items);

        set_menu_win(job_menu, win);
        set_menu_sub(job_menu, derwin(win, 4, 15, y, x));

	menu_opts_off(job_menu, O_SHOWDESC);
        set_menu_fore(job_menu, COLOR_PAIR(PUR_YEL));
        set_menu_back(job_menu, COLOR_PAIR(PUR_PUR));
        set_menu_format(job_menu, 4, 1);
        set_menu_mark(job_menu, "");

        post_menu(job_menu);
        vrt_refresh();
}
