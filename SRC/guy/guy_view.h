void view_dock(void);
void view_vitals(int operand);
void slativ_weiv(void);
void view_attributes(void);
void view_noun(void);
void view_noun_grey(void);
void *get_pattern(void);

void view_nouns(void);
void close_nouns(void);

MENU *get_noun_menu(void);

void install_key(uint32_t key, int option);
enum install_key_opts {KEY_SUBJECT, KEY_OBJECT};
