void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0);
void shift_highlights(PLATE *pl);
void highlights_init(PLATE *pl);
void gen_terrain(PLATE *pl, char type, int h, int w, int y0, int x0);

void connected_components(PLATE *pl, int type);
void simpledraw(PLATE *pl);

void unfuck_perspective(PLATE *pl);

void testworld(int size_factor);
