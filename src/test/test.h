extern WINDOW *SYSWIN;
extern PANEL  *SYSPAN;

extern WINDOW *MARQUEEW;
extern PANEL  *MARQUEEP;

extern WINDOW *DIAGNOSTIC_WIN;
extern PANEL  *DIAGNOSTIC_PAN;

extern WINDOW *BIGWIN;
extern PANEL  *BIGPAN;

extern WINDOW *INSPECTORMSGWIN;
extern PANEL  *INSPECTORMSGPAN;

void init_test(void);
void toggle_dpanel(void);
void speak_error(const char *error);
void toggle_syspan(void);
void speak_error_mono(const char *error);
void toggle_bigpan(void);
int inspect_control(int dir);
