struct prog {
    char *name;
    unsigned char *code;
    unsigned len;
    unsigned syncpoint;
};
struct prog *menu_pick(void);
