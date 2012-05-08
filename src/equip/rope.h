

struct rope_t {
        int id;
        int len;
        int test;
        struct cell_t *head;
        struct cell_t *end;
};

uint32_t ropekey;

uint32_t new_rope(int len, int test);
void rope_head(int y, int x, uint32_t key);
void rope_drop(uint32_t key);


