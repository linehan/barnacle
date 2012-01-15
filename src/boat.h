
int boat_init(void);

void nominate_boat(MOB *mob);

MOB *new_boat(void);

void sync_boat(void);

int get_boat(const char *str1, const char *str2);
void set_boat(const char *str1, const char *str2, int val);

void *sail_boat(void *);

void order_boat(int, int);

int get_pointofsail(void);



void deck_init(void);

void draw_deck(void);

void tog_deck(void);
