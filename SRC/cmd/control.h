extern PANEL *deckp;
enum cardinal { 
        NORTH = 0,
        NNE   = 1,
        NE    = 2,
        ENE   = 3,
        EAST  = 4,
        ESE   = 5,
        SE    = 6,
        SSE   = 7,
        SOUTH = 8,
        SSW   = 9,
        SW    = 10,
        WSW   = 11,
        WEST  = 12,
        WNW   = 13,
        NW    = 14,
        NNW   = 15 
};

void *asyncio(void *ptr);

void bark(int order, int value);
int unvim(int v);
