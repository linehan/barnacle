enum wind_options{
        __dir__ = 0, 
        __str__ = 1, 
        __pre__ = 2 
};
void init_weather(void);

int get_wind(int a);

void set_wind(int a, int val);
//int wind_roll(void);

void wind_gfx(void);
//void *tumble_compass(int a);

extern WINDOW *DIAGNOSTIC_WIN;
extern PANEL *DIAGNOSTIC_PAN;
