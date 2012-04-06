
typedef struct climate_package {
        int lat;
        int temp;
        int wind;
        int elev;
        int celltype;
        int cellspan;
        int cellgrade;
        int precip;
        int humid;
} CLIM;

void init_weather(void);
int get_wind(int a);
void set_wind(int a, int val);
void wind_gfx(void);
