void init_ctrlpanels(void);
WINDOW *INFOW[4];
PANEL  *INFOP[4];

#define W_BORD COLS
#define H_BORD 25 

#define X_PADD 2
#define Y_PADD 1
#define W_PADD W_BORD-(2*X_PADD)
#define H_PADD H_BORD-(2*Y_PADD) 

#define X_BORD 0
#define Y_BORD 0

#define X_BODY 2 
#define Y_BODY 3 
#define W_BODY W_PADD-(2*X_BODY)
#define H_BODY H_PADD-(2*Y_BODY)

#define X_HEAD 2
#define Y_HEAD 1
#define W_HEAD W_PADD-(2*X_HEAD)
#define H_HEAD H_PADD-(2*Y_HEAD)

enum info_panel_enum { BORD, PADD, HEAD, BODY };

#define TOGGLE_INFO             \
        TOGPAN(INFOP[BORD]);    \
        TOGPAN(INFOP[PADD]);    \
        TOGPAN(INFOP[HEAD]);    \
        TOGPAN(INFOP[BODY]);    


PANEL *purple_panel(WINDOW *win[4], int h, int w, int y, int x);
