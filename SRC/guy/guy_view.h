
void view_dock(void);
void view_vitals(void);
void view_attributes(void);
void view_noun(void);
void view_noun_grey(void);
void *get_pattern(void);

void view_nouns(void);
void close_nouns(void);

extern MENU *noun_menu;
extern uint32_t active_key;

static inline void install_key(uint32_t key)
{
        active_key = key;
}

