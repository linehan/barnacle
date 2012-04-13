void view_vitals(int operand);
void view_attributes(int operand);
void view_noun(int operand);
void view_noun_grey(int operand);
void *pattern_noun_menu(int op);

void close_noun_menu(int operand);
void open_noun_menu(int operand);
void tog_noun_menu(int operand);

MENU *get_noun_menu(int operand);
void list_nouns(int which, int opt);

enum list_noun_opts {ALL_NOUNS};


