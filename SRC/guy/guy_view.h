void view_dock(void);
void view_vitals(int operand);
void slativ_weiv(void);
void view_attributes(void);
void view_noun(int operand);
void view_noun_grey(int operand);
void *get_pattern(void);

void close_nouns(int operand);
void open_nouns(int operand);

MENU *get_noun_menu(int operand);

void install_key(uint32_t key, int option);
enum install_key_opts {KEY_SUBJECT, KEY_OBJECT};
