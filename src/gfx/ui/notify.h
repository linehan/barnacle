
#define FOUND(x) ("Found " ## (x))

enum alerts {
        I_FIND,
        I_KILL,
};

void do_gloss(void);
void say(const wchar_t *who, const char *msg);
void alert(enum alerts tag, char *msg);
