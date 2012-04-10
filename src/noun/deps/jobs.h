#pragma once
#ifndef __JOBS_MODEL
#define __JOBS_MODEL

#define JOB_COUNT 51

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

static inline const char *get_job(int id)
{
        focus(id);
        return (job_list[focused->job]);
}

#endif
