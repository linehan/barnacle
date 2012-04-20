
WINDOW *CONSOLE_WIN;
PANEL  *CONSOLE_PAN;

WINDOW *DIAGNOSTIC_WIN;
PANEL  *DIAGNOSTIC_PAN;

WINDOW *HUD_WIN;
PANEL  *HUD_PAN;

WINDOW *INSPECTORWIN;
PANEL  *INSPECTORPAN;
WINDOW *INSPECTORMSGWIN;
PANEL  *INSPECTORMSGPAN;

void init_test(void);
int inspect_control(int dir);
void test_request(void);

void lay_hud(void);
void hud_win(WINDOW *win);
