void set_gfx_bg(ENV *env, int opt);
void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0);
void shift_highlights(void);
void highlights_init(ENV *env);
void gen_terrain(ENV *env, char type, int h, int w, int y0, int x0);