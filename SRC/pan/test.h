
extern WINDOW *SYSWIN;
extern PANEL  *SYSPAN;
extern WINDOW *DIAGNOSTIC_WIN;
extern PANEL *DIAGNOSTIC_PAN;
extern WINDOW *BIGWIN;
extern PANEL *BIGPAN;
void test_init(void);
void toggle_dpanel(void);
void speak_error(const char *error);
void toggle_syspan(void);
void speak_error_mono(const char *error);
void toggle_bigpan(void);
