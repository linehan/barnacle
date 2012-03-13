#ifndef __CREATURES
#define __CREATURES

#include <string.h>
#include "../eng/state.h"



struct guy_t {
        char fname[30];
        char mname[30];
        char lname[30];
        char birth[30];
        uint8_t job;
        uint8_t age;
        uint8_t weight;
        uint8_t height;
        uint8_t gender;
        uint8_t xpulse;
        bool positive;
        uint32_t vitals;
        uint32_t stats;
};


/*
  Creatures have an 'attributes' member, which is a packaged state consisting
  of 8 nibbles of a 32-bit uint32_t. They will be familiar to anybody with a
  passing acquaintence with D&D.
*/
enum attr_enum {STR,DEX,VIT,AGI,INT,WIS,CHA,LUC};
static const wchar_t attr_sym[]={L"ΣΦΔAΨWΧΛ"};

struct rb_tree *GUY_TREE; // Holds all creatures
uint32_t muster[100];     // Holds keys of all creatures


static struct guy_t *focused;
static inline void focus(uint32_t key)
{
        focused = (struct guy_t *)rb_extra(GUY_TREE, key);
}



void load_guy_test(void);
void attr_see(wchar_t *wch, uint32_t key);
void attr_pkg(int *out, uint32_t key);
uint32_t get_cstat(uint32_t key, int n);
void set_cstat(uint32_t key, int n, int v);
char *getjob(uint32_t key);
char *fname(uint32_t key);
char *mname(uint32_t key);
char *lname(uint32_t key);
char *flname(uint32_t key);
char *fullname(uint32_t key);
uint32_t *vit_getpkg(uint32_t key);
int npersons(void);

uint32_t new_guy(char *firname,
                char *midname,
                char *lasname,
                char *birthpl,
                uint8_t job,
                uint8_t age,
                uint8_t weight,
                uint8_t height,
                uint8_t gender);

#endif
