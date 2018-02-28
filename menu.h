struct prog {
    char *name;
    unsigned char btnmap[4];
    unsigned char *code;
    unsigned len;
};
struct prog *menu_pick(void);
