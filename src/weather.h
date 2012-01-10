void weather_init(void);
void wind_set(int direction);
int wind_get(void);

int wind_roll(void *ptr);
int get_wind_dir(void);
int get_wind_str(void);

void wind_gfx(void);

void set_prevailing(int dir, double factor);
