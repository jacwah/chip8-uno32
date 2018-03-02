enum dmode {
    DMODE_RAW,
    DMODE_TILT,
};

struct prog {
    char *name;
    unsigned char *code;
    unsigned len;
    unsigned syncpoint;
    unsigned char *keymap;
    enum dmode dmode;
};
struct prog *menu_pick(void);
