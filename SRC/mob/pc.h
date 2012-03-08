#define MAX_PEOPLE 100
uint32_t person_list[MAX_PEOPLE];

uint32_t get_cstat(uint32_t key, int n);
void set_cstat(uint32_t key, int n, int v);
void inc_cstat(uint32_t key, int n);
void dec_cstat(uint32_t key, int n);
char *fname(uint32_t key);
char *mname(uint32_t key);
char *lname(uint32_t key);
char *fullname(uint32_t key);
int npersons(void);

uint32_t new_pc(char *firname,
                char *midname,
                char *lasname,
                char *birthpl,
                uint8_t job,
                uint8_t age,
                uint8_t weight,
                uint8_t height,
                uint8_t gender);
