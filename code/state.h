struct inputstate {
    char next_turn;
    struct {
        float x;
        float y;
        uint8_t buttons;
    } mouse;
    struct {
        char up;
        char down;
        char left;
        char right;
    } arrows;
};

struct framestate {
    mat4 rot;
    mat4 mov;
    struct {
        float x;
        float y;
    } pan;
    struct {
        float x;
        float y;
    } mouse;
    char lock;
    unsigned turn;
    float turn_tail;
    int turn_transition;
    double turn_transition_ends;

    mat4 proj;
    mat4 viewi;
    mat4 viewproj;

    double time;
};
