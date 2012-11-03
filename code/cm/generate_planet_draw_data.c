struct planet_draw_data
generate_planet_draw_data (
    double time,
    struct planet const * planet,
    struct frame_draw_data const * framedata
) {
    float phi = (float) ((time / planet->orbit.period) * pi () * 2.0);

    mat4 mmodel = planet->orbit.matrix;
    mmodel.column.w.element.x += planet->orbit.major * cosf (phi);
    mmodel.column.w.element.y += planet->orbit.minor * sinf (phi);

    vec3 first = vec3_diff (
        & mmodel.column.w.v3,
        & framedata->viewi.column.w.v3);

    float p = vec3_length (& first);
    float r = planet->size;
    float apparent = sqrtf (p * p - r * r) * r / p;
    float apparentratio = apparent / r;
    float offset = (r * r) / p;
    float tosurface = p - r;
    float hack = logf (tosurface) / 1000.0f;

    vec3 unit_x = {{1.0f, 0.0f, 0.0f}};
    vec3 unit_y = {{0.0f, 1.0f, 0.0f}};

    vec3 second = first.element.x < first.element.y ?
        vec3_product (& first, & unit_x) :
        vec3_product (& first, & unit_y) ;

    vec3 third = vec3_product (& first, & second);

    mat4 rotation = {.p[15] = 1.0f};
    rotation.column.z.v3 = vec3_normalized (& first);
    rotation.column.x.v3 = vec3_normalized (& second);
    rotation.column.y.v3 = vec3_normalized (& third);
    mmodel = mat4_multiply (& mmodel, & rotation);

    mat4 rot = mmodel;
    rot.column.w.v3 = (vec3) {{0}};

    vec3 move = {{0.0f, 0.0f, -offset}};
    mmodel = mat4_moved (& mmodel, & move);
    mmodel = mat4_scaled (& mmodel, apparent);

    mat4 mvp = mat4_multiply (& framedata->viewproj, & mmodel);

    struct planet_draw_data data = {
        mvp,
        rot,
        hack,
        apparentratio,
        0, /* ? */
        planet->color
    };

    return data;
}
