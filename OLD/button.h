struct button_t {
        WINDOW *win;
        char message[30];
        short pair[3];
};

struct button_t *new_button(const char *message, short norm, short hi, short grey);
void put_button(struct button_t *button, WINDOW *win, int h, int w, int y, int x);
void focus_button(struct button_t *button);
void post_button(struct button_t *button);
void unfocus_button(struct button_t *button);
