
extern WINDOW *SYSWIN;
extern PANEL  *SYSPAN;
extern WINDOW *DIAGNOSTIC_WIN;
extern PANEL *DIAGNOSTIC_PAN;
void test_init(void);
void toggle_dpanel();
void speak_error(const char *error);
void toggle_syspan(void);
void speak_error_mono(const char *error);
