#pragma once
#ifndef __MAP_H
#define __MAP_H

#include <stdint.h>
#include "../gfx/gfx.h"
#include "../eng/bytes.h"
#include "../lib/ufo.h"

#define NLAYERS 16  // Number of layers on the world map.

struct map_t {
        WINDOW *win;
        PANEL  *pan;
        struct multiwin_t *L[NLAYERS];
        struct multiwin_t *W;
        struct matrix_t *mx;
        struct ufo_t ufo;
        double **pmap;
        void (*render)(void *self);
        void (*restack)(void *self);
};

extern struct map_t *GLOBE; // Global map.

//{{{1 NIBBLE GUIDE 
enum map_nibs {LAY=0,ALT=1,BED=2,SED=3,SOI=4,MOI=5,TEM=6,WND=7};
static const 
char *map_nibs[8] = {"LAY","ALT","BED","SED","SOI","MOI","TEM","WND"};
//}}}1
//{{{1 LAY -- layers
enum lay_t map_layers;
//}}}1
//{{{1 ALT -- altitude
enum ALT_state_tags {                                                   
        //XXX = 0, 
        HIG4 = 1,  // highgest
        HIG3 = 2, 
        HIG2 = 3, 
        HIG1 = 4,  
        HIG0 = 5, 
        LOW4 = 6,  // lowest 
        LOW3 = 7,   
        LOW2 = 8,  
        LOW1 = 9,  
        LOW0 = 10,
        DROP = 11, // a hole going down
        CLIF = 12  // a cliff face going up
};
static const 
char *ALT_tag[16] = {"XXXX", "HIG4", "HIG3", "HIG2", "HIG1", "HIG0", 
                      "LOW4", "LOW3", "LOW2", "LOW1", "LOW0", "DROP",
                      "CLIF", "XXXX", "XXXX"};
//}}}1
//{{{1 BED -- bedrock
enum BED_state_tags {
        //XXX = 0,
        GRAN = 1,  // granite
        SCHI = 2,  // schist
        GABB = 3,  // gabbro
        BASA = 4,  // basalt
        ANDE = 5,  // andesite
        DIOR = 6,  // diorite
        HORN = 7,  // hornblende
        TUFF = 8,  // volcanic tuff
        MARB = 9,  // marble
        FELS = 10  // felsite
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *BED_tag[16] = {"XXXX", "GRAN", "SCHI", "GABB", 
                     "BASA", "ANDE", "DIOR", "HORN", 
                     "TUFF", "MARB", "FELS", "XXXX", 
                     "XXXX", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 SED -- sedimentary/clastic rock
enum SED_state_tags {                                                   
        //XXX = 0,
        BREC = 1,  // breccia
        CHAL = 2,  // chalk
        CHER = 3,  // chert
        CLST = 4,  // claystone
        DOLO = 5,  // dolomite
        WACK = 6,  // greywacke
        LIME = 7,  // limestone
        MUDS = 8,  // mudstone
        SHAL = 9,  // shale
        SILS = 10, // siltstone
        TRAV = 11  // travertine
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *SED_tag[16] = {"XXXX", "BREC", "CHAL", "CHER", 
                     "CLST", "DOLO", "WACK", "LIME", 
                     "MUDS", "SHAL", "SILS", "TRAV", 
                     "XXXX", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 SOI -- topsoil
enum SOI_state_tags {                                                   
        //XXX = 0,
        ENTI = 1,  // entisol    - recently formed soil; unconsolidated
        VRTI = 2,  // vertisol   - inverted soil; swell when wet, crack
        INCP = 3,  // inceptisol - young soil; little eluviation/illuviation
        ARID = 4,  // aridisol   - dry soil; scarce organics
        MOLL = 5,  // mollisol   - soft soil with thick A horizons
        SPOD = 6,  // spodisol   - soil produced by podsolization; forests
        ALFI = 7,  // alfisol    - soil with Al and Fe; clay accumulation
        ULTI = 8,  // ultisol    - soil that are heavily leached
        OXIS = 9,  // oxisol     - soil with heavy oxide content
        HIST = 10, // histosol   - organic soil
        ANDI = 11, // andisol    - volcanic soil; high in glass content
        GELI = 12  // gelisol    - permafrost soil
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *SOI_tag[16] = {"XXXX", "ENTI", "VRTI", "INCP", 
                     "ARID", "MOLL", "SPOD", "ALFI", 
                     "ULTI", "OXIS", "HIST", "ANDI", 
                     "GELI", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 MOI -- moisture content
enum MOI_state_tags {                                                   
        WET5 = 1,  // top
        WET4 = 2,  // bottom
        WET3 = 3,  // left
        WET2 = 4,  // right 
        WET1 = 5,  // right 
        WET0 = 6,  // front 
        DRY5 = 7,  // reserved
        DRY4 = 8,  // back 
        DRY3 = 9,  // reserved
        DRY2 = 10, // reserved
        DRY1 = 11, // reserved
        DRY0 = 12  // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const 
char *MOI_tag[16] = {"XXXX", "WET5", "WET4", "WET3", 
                     "WET2", "WET1", "WET0", "DRY5", 
                     "DRY4", "DRY3", "DRY2", "DRY1", 
                     "XXXX", "XXXX", "XXXX", "XXXX"};
//}}}1
//{{{1 TEM -- temperature
enum grad_t temperature;
//}}}1
//{{{1 WND -- wind heading
enum hdg_t wind_direction;
//}}}1
//{{{1 OPTIONS INDEX
static const
char **map_opts[8] = {lay_tag,ALT_tag,BED_tag,SED_tag,SOI_tag,MOI_tag,grad_tag,hdg_tag};
//}}}1

#define PLATE(map, tag) PEEK(((map)->L[(tag)]))

struct map_t *new_map(int h, int w, int scr_h, int scr_w, int scr_y, int scr_x);
int           map_hit(struct map_t *map, struct rec_t *rec);
void          map_gen(struct map_t *map, double **pmap);
void         map_roll(struct map_t *map, int dir);

#endif
