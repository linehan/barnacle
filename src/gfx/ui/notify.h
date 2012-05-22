

enum alerts {
        I_FIND,
        I_KILL,
};

void say(const wchar_t *who, const char *msg);
void alert(enum alerts tag, char *msg);
