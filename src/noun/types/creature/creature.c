#include "../../../com/arawak.h"

#include "creature.h"
#include "../../noun_model.h"
#include "../../../eng/fsm.h"



// med left ᱙
// med left Ꮆ
// strong left ᕦ

// strong right ᕤ
// med right ᱽ

// ᑺ ᑻ ᑼ ᑽ ᒇ ᑯ ᑰ ᑱ 
// ᑾ ᑿ ᒀ ᒁ ᒂ ᒈ ᑲ ᑳ
// ᑬ ᑭ ᑮ ᑶ ᑷ ᑸ ᑹ
// ᑫ ᑴ ᑵ
// ᓀ ᓉ ᓊ
// ᓄ ᓅ ᓆ
// ᓁ ᓂ ᓃ 
// ᓇ ᓈ ᓋ ᓌ ᓍ ᓎ ᓏ

struct animation punch_r_test = { 
{L"Ꮂ", L"Ꮂ", L"Ꮂ", L"ᱽ", L"ᕤ", L"ᱽ", L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 
10
};

struct animation punch_l_test = { 
{L"Ꮂ", L"Ꮂ", L"Ꮂ", L"᱙", L"ᕦ" L"᱙", L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 
10
};

struct animation slashtest = { 
{L"Ꮂ", L"Ꮂ", L"Ꮂ", L"ᒀ", L"ᒀ", L"ᒀ", L"ᒀ", L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 
11
};

struct animation dodgetest = { 
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"Ꮱ",L"Ꮱ",L"Ꮱ",L"Ꮱ",L"ȣ",L"ȣ",L"ȣ",L"ȣ",L"Ꮱ",L"Ꮂ",L"Ꮂ",L"ⰾ",0}, 
16, 9, 'd'
};

struct animation falltest = { 
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"ޗ",L"ޗ",L"ޗ",L"ⲁ",L"ⲁ",L"ⲁ",L"ᥑ", 0}, 
13, 5, 'r'
};

struct animation dodge_l_test = {
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"ᥑ",L"ᥑ",L"ⲁ",L"ཚ",L"ཚ",L"ཚ",L"Ꮂ",L"Ꮂ",L"Ꮂ"L"ⰾ",0},
14, 6, 'l'
};

struct animation dodge_r_test = {
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"ᥑ",L"ᥑ",L"ⲁ",L"ཚ",L"ཚ",L"ཚ",L"Ꮂ",L"Ꮂ",L"Ꮂ"L"ⰾ",0},
14, 6, 'r'
};

struct animation kick_r_test = {
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"࿂",L"࿂",L"ᱯ",L"࿂",L"࿂",L"༱",L"༱",L"Ꝭ",L"Ꝭ",L"ỿ",L"ᓀ",L"ᓀ",L"Ꮂ",L"ⰾ",0},
19, 10, 'r'
};

struct animation kick_l_test = {
{L"Ꮂ",L"Ꮂ",L"Ꮂ",L"࿂",L"࿂",L"ᱯ",L"ᱯ",L"࿂",L"࿂",L"༱",L"Ꝭ",L"Ꝭ",L"Ꝭ",L"ᓂ",L"ᓂ",L"Ꮂ",L"ⰾ",0},
19, 10, 'l'
};

struct animation run_u_test = { {L"Ꮂ", L"ⲑ", L"ⲑ", L"ⱚ", L"ⰾ", 0}, 6, 1, 'u'};
struct animation run_d_test = { {L"Ꮂ", L"ⲑ", L"ⲑ", L"ⱚ", L"ⰾ", 0}, 6, 1, 'd'};
struct animation run_l_test = { {L"Ꮂ", L"ⲑ", L"ⲑ", L"ⱚ", L"ⰾ", 0}, 6, 1, 'l'};
struct animation run_r_test = { {L"Ꮂ", L"ⲑ", L"ⲑ", L"ⱚ", L"ⰾ", 0}, 6, 1, 'r'};

/*struct animation dodgetest = { {L"Ꮱ", L"Ꮱ", L"ȣ", L"Ꮱ", L"Ꮂ", L"ⰾ", 0}, 7, 3, 'd'};*/
/*struct animation falltest = { {L"ᥑ", L"ᥑ", L"ⲁ", L"ⴋ", L"ᥑ", 0}, 7, 3, 'l'};*/


/*struct animation run_u1_test = { {L"ⲑ",L"ⲑ", L"ⰾ", 0}, 5, 2, 'u'};*/
/*struct animation run_u2_test = { {L"ϯ", L"ϯ", L"ⰾ", 0}, 5, 2, 'u'};*/
/*ϯⲑⰪⱀⱚ*/
/*L"ϯ", L"ϯ"*/
/*ⰾⰄⰴⰾ*/
/*ⰾᎲⰴⰾ*/
/*struct animation u_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'u'};*/
/*struct animation d_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'd'};*/
/*struct animation l_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'l'};*/
/*struct animation r_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'r'};*/
/*ⰾᎲᱽᎲⰾ*/
/*ⰾᏡȣᎲⰾ*/



void new_creature(const char *name, int job, const wchar_t *wch, short pair)
{
        struct person_t *new;
        struct noun_t *noun;

        new = malloc(sizeof(struct person_t)); 

        noun = new_noun(name, job, 0, new);
        noun_set_modify(noun, modify_creature);
        mob_cfg(&noun->mob, FIELD, 1, 1, CENTERED);

        noun->options |= NOUN_CREATURE;

        /*init_pair(pair, BLACK, WHITE);*/
        setcchar(&new->cch, wch, 0, pair, NULL);
        wbkgrnd(mob_win(&noun->mob), &new->cch);

        noun->mob.animate = NULL;
        noun->mob.punch_l = &punch_l_test;
        noun->mob.punch_r = &punch_r_test;
        noun->mob.dodge   = &dodgetest;
        noun->mob.kick_r = &kick_r_test;
        noun->mob.kick_l = &kick_l_test;
        noun->mob.dodge_r = &dodge_r_test;
        noun->mob.dodge_l = &dodge_l_test;
        noun->mob.fall = &falltest;
        noun->mob.slash = &slashtest;
        noun->mob.run_u = &run_u_test;
        noun->mob.run_d = &run_d_test;
        noun->mob.run_l = &run_l_test;
        noun->mob.run_r = &run_r_test;
}



static bool whichrun;

int modify_creature(void *obj, int input)
{
        struct noun_t *noun = (struct noun_t *)obj;

        switch(input) 
        { 
        /*case 'j':*/
        /*case 's':*/
                /*mob_move(&noun->mob, 'd');*/
                /*break;*/
        /*case 'k':*/
        /*case 'w':*/
                /*mob_move(&noun->mob, 'u');*/
                /*break;*/
        /*case 'h':*/
        /*case 'a':*/
                /*[>mob_move(&noun->mob, 'l');<]*/
                /*break;*/
        /*case 'l':*/
        /*case 'd':*/
                /*[>mob_move(&noun->mob, 'r');<]*/
                /*break;*/
        case 'j':
        case 's':
        case 'J':
        case 'S':
                noun->mob.animate = noun->mob.run_d;
                break;
        case 'k':
        case 'w':
        case 'K':
        case 'W':
                noun->mob.animate = noun->mob.run_u;
                break;
        case 'h':
        case 'a':
        case 'H':
        case 'A':
                noun->mob.animate = noun->mob.run_l;
                break;
        case 'l':
        case 'd':
        case 'L':
        case 'D':
                noun->mob.animate = noun->mob.run_r;
                break;
        /*case 't':*/
                /*mob_seek(noun_mob("Beefman"), &noun->mob);*/
                /*break;*/
        case 'g':
                top_panel(noun->mob.pan);
                break;
        case 'r':
                noun->mob.animate = noun->mob.punch_l;
                break;
        case 't':
                noun->mob.animate = noun->mob.punch_r;
                break;
        case ' ':
                noun->mob.animate = noun->mob.dodge;
                break;
        case 'e':
                noun->mob.animate = noun->mob.slash;
                break;
        case '|':
                noun->mob.animate = noun->mob.fall;
                break;
        case ']':
                noun->mob.animate = noun->mob.kick_r;
                break;
        case '[':
                noun->mob.animate = noun->mob.kick_l;
                break;
        case KEY_ESC:
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}



