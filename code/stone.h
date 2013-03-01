struct stone_G1 {
    mat4 transform;
    float supersize;
    float size;
};

struct stone_G2 {
    mat4 mvp;
    mat4 mv;
    float depth;
    float uvscale;
    GLuint texture;
    vec3 colour;
};

struct stone_engine {
    struct GL * gl;
    struct SDL * sdl;
    struct hot_player * H;

    struct galaxy * G;
    struct stone_G1 * G1;
    struct stone_G2 * G2;

    struct framestate * S;

    GLuint tex;
    GLuint cell_vbo;
    struct GLvbo_and_size imposter;
    struct glts_planeta sh_pl [3];
    struct glts_cello sh_ce;
};
