struct prog {
    char *name;
    unsigned char *code;
    unsigned len;
    unsigned syncpoint;
    unsigned char *keymap;
};
struct prog *menu_pick(void);
