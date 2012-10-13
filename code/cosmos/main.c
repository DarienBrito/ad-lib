int main (int argc, char * argv []) {
    (void) argc;
    (void) argv; /* silence the warnings */

    int error = 0;
    int sdlerror = 0;

    logi ("Revving up.");

    GLuint prog = GL_FALSE;
    GLuint vs = GL_FALSE;
    GLuint fs = GL_FALSE;
    int gl_initted = 0;

    DIR * sysdir = NULL;

    TAILQ_HEAD (head, sysplanet) list;
    TAILQ_INIT (&list);

    struct shader_t vss = {GL_VERTEX_SHADER, &vs, &prog};
    struct shader_t fss = {GL_FRAGMENT_SHADER, &fs, &prog};

    struct settings settings;
    if (loadsettings (&settings, "data/settings") != 0) {
        error = __LINE__;
        goto end;
    }

    int img_require = IMG_INIT_JPG;
    int img_initted = IMG_Init (img_require);

    if ((img_require & img_initted) != img_require) {
        logi ("SDL_image error: %s", IMG_GetError ());
        error = __LINE__;
        goto end;
    }

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        sdlerror = 1;
        error = __LINE__;
        goto end;
    }

    SDL_Window * window = SDL_CreateWindow ("Cosmos",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        settings.width, settings.height, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL) {
        sdlerror = 1;
        error = __LINE__;
        goto end;
    }

    if (SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2) != 0 ||
        SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 0) != 0)
    {
        sdlerror = 1;
        error = __LINE__;
        goto end;
    }

    SDL_GLContext context = SDL_GL_CreateContext (window);

    if (context == NULL) {
        sdlerror = 1;
        error = __LINE__;
        goto end;
    }

    gl_initted = gl_init ();
    if (gl_initted != 0) {
        sdlerror = 1;
        error = __LINE__;
        goto end;
    }

    prog = glCreateProgram ();

    if (loadshader (&vss, "data/shade/planet.vert") != 0) {
        error = __LINE__;
        goto end;
    }
    if (loadshader (&fss, "data/shade/planet.frag") != 0) {
        error = __LINE__;
        goto end;
    }

    glDeleteShader (vs);
    glDeleteShader (fs);

    GLint code = GL_FALSE;
    glGetProgramiv (prog, GL_LINK_STATUS, &code);

    if (code == GL_FALSE) {
        error = __LINE__;
        goto end;
    }

    glUseProgram (prog);

    struct {
        float x;
        float y;
    } const tris [] = {
        {-1.0,-1.0},
        { 1.0,-1.0},
        {-1.0, 1.0},
        {-1.0, 1.0},
        { 1.0,-1.0},
        { 1.0, 1.0}
    };
    int const vertices = sizeof (tris) / sizeof (tris[0]);

    GLuint vbo = GL_FALSE;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (tris), tris, GL_STATIC_DRAW);

    char const * const earthimg [6] = {
        "art/earth/posx.jpg",
        "art/earth/negx.jpg",
        "art/earth/posy.jpg",
        "art/earth/negy.jpg",
        "art/earth/posz.jpg",
        "art/earth/negz.jpg",
    };

    SDL_Surface * earth [6];
    for (int i = 0; i < 6; ++i) {
        earth[i] = IMG_Load (earthimg[i]);
        if (earth[i] == NULL) {
            logi ("SDL_image error: %s", IMG_GetError ());
            error = __LINE__;
            goto end;
        }
    }

    Uint32 sdlformat = earth[0]->format->format;
    GLenum glformat = GL_FALSE;
    if (sdlformat == (Uint32) SDL_PIXELFORMAT_RGB24) {
        glformat = GL_RGB;
    } else if (sdlformat == (Uint32) SDL_PIXELFORMAT_RGB888) {
        glformat = GL_BGRA;
    } else {
        logi ("Unexpected texture format: %s",
            SDL_GetPixelFormatName (sdlformat)); 
        error = __LINE__;
        goto end;
    }

    GLuint tex = GL_FALSE;
    glGenTextures (1, &tex);
    glBindTexture (GL_TEXTURE_CUBE_MAP, tex);

    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -1.75f);

    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexParameteri
        (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexParameteri
        (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum targets [6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    for (int i = 0; i < 6; ++i) {
        glTexImage2D (targets[i], 0, GL_RGBA, earth[i]->w, earth[i]->h,
            0, glformat, GL_UNSIGNED_BYTE, earth[i]->pixels);
    }

    for (int i = 0; i < 6; i++) {
        SDL_FreeSurface (earth[i]);
    }

    glActiveTexture (GL_TEXTURE0);

    glEnable (GL_DEPTH_TEST);
    glViewport (0, 0, settings.width, settings.height);

    GLuint const attribute_pos = (GLuint) glGetAttribLocation (prog, "pos");
    if (attribute_pos == (GLuint) -1) {
        error = __LINE__;
        goto end;
    }

    glVertexAttribPointer (attribute_pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (attribute_pos);

    GLint const uniform_depth = glGetUniformLocation (prog, "depth");
    if (uniform_depth == -1) {
        logi ("GL uniform 'depth' not found");
    }

    GLint const uniform_mvp = glGetUniformLocation (prog, "mvp");
    if (uniform_mvp == -1) {
        logi ("GL uniform 'mvp' not found");
    }

    GLint const uniform_mv = glGetUniformLocation (prog, "mv");
    if (uniform_mv == -1) {
        logi ("GL uniform 'mv' not found");
    }

    GLint const uniform_color = glGetUniformLocation (prog, "color");
    if (uniform_color == -1) {
        logi ("GL uniform 'color' not found");
    }

    GLint const uniform_uvscale = glGetUniformLocation (prog, "uvscale");
    if (uniform_uvscale == -1) {
        logi ("GL uniform 'uvscale' not found");
    }

    GLint const uniform_texture = glGetUniformLocation (prog, "texture");
    if (uniform_texture == -1) {
        logi ("GL uniform 'texture' not found");
    }

    float mori [4 * 4];
    identitymatrix (mori);

    float mcam [4 * 4];
    identitymatrix (mcam);
    vec3 axisx = {1.0f, 0.0f, 0.0f};
    rotatematrix (mcam, M_PI / 2.4f, & axisx);
    float move [3] = {0.0f, 0.0f, -13.0f};
    translatematrix (mcam, move);
    vec3 axisz = {0.0f, 0.0f, 1.0f};
    rotatematrix (mcam, M_PI, & axisz);

    char const * const dirname = "data/spawn";
    sysdir = opendir (dirname);
    if (sysdir == NULL) {
        error = __LINE__;
        goto end;
    }

    struct dirent * dirent = NULL;
    while ((dirent = readdir (sysdir)) != NULL) {
        if (dirent->d_name[0] != '.') {
            size_t len = strlen (dirname) + 1 + strlen (dirent->d_name) + 1;
            struct sysplanet * item = (struct sysplanet *) malloc (sizeof (*item));
            assert (item != NULL);
            item->file = (char *) malloc (len);
            assert (item->file != NULL);
            
            snprintf (item->file, len, "%s/%s", dirname, dirent->d_name);
            TAILQ_INSERT_TAIL (&list, item, _);

            if (loadplanet (&item->planet, item->file) != 0) {
                error = __LINE__;
                goto end;
            }
        }
    }

    float x = 0;
    float y = 0;
    int mouselock = 0;
    for (;;) {
        GLuint glerror = glGetError ();
        if (glerror != 0) {
            logi ("GL error # %d.", glerror);
            error = __LINE__;
            goto end;
        }

        SDL_Event evt;
        if (SDL_PollEvent (&evt)) {
            if (evt.type == SDL_KEYDOWN) {
                SDL_Keycode key = evt.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    goto end;
                }
            } else if (evt.type == SDL_QUIT) {
                goto end;
            }
        }

        int ix, iy;
        Uint8 mousebuttons = SDL_GetMouseState(&ix, &iy);

        float norm = settings.width > settings.height ?
            settings.width : settings.height;
        float nx = ix / norm;
        float ny = iy / norm;
        float dx;
        float dy;
        if (mouselock != 0) {
            dx = x - nx;
            dy = y - ny;
        } else {
            dx = 0;
            dy = 0;
        }
        x = nx;
        y = ny;

        mouselock = mousebuttons;

        float const ROTATION_SPEED = 4.0f;
        float angle = sqrtf (dx * dx + dy * dy) * ROTATION_SPEED;
        vec3 axis = {dy, 0.0f, dx};

        rotatematrix (mori, angle, & axis);

        double time = (double) SDL_GetTicks () / 1000;

        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float mview [4 * 4];
        memcpy (mview, mori, sizeof (mview));
        multiplymatrix (mview, mcam);
        invertspecialmatrix (mview);

        float mviewi [4 * 4];
        memcpy (mviewi, mview, sizeof (mview));
        invertspecialmatrix (mviewi);

        struct sysplanet * item;
        TAILQ_FOREACH(item, &list, _) {
            float tosurface;
            float apparentratio;
            float mmodel [4 * 4];
            float mrot [4 * 4];
            planetmatrix (&item->planet, time, mviewi, mmodel, mrot,
                &tosurface, &apparentratio);

            float matrix [4 * 4];

            float const aspect = ((float) settings.width) / settings.height;
            projectionmatrix (settings.fov, aspect, 0.0f, matrix);
            multiplymatrix (matrix, mview);
            multiplymatrix (matrix, mmodel);
            glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, matrix);

            glUniformMatrix4fv (uniform_mv, 1, GL_FALSE, mrot);

            float const hack = logf (tosurface) / 1000.0f;
            glUniform1f (uniform_depth, hack);

            glUniform1f (uniform_uvscale, apparentratio);
            glUniform3fv (uniform_color, 1, item->planet.color);

            glUniform1i (uniform_texture, 0);

            glDrawArrays (GL_TRIANGLES, 0, vertices);
        }
 
        SDL_GL_SwapWindow (window);
    }
 
    end:

    {
        struct sysplanet * item;
        struct sysplanet * tvar;

        TAILQ_FOREACH_SAFE(item, &list, _, tvar) {
            free ((void *) item->file);
            TAILQ_REMOVE (&list, item, _);
            free ((void *) item);
        }
    }

    if (sysdir != NULL) {
        closedir (sysdir);
    }

    if (gl_initted != 0) {
        glDeleteBuffers (1, &vbo);
        glDeleteTextures (1, &tex);
        glDeleteProgram (prog);
    }

    if (sdlerror != 0) {
        logi ("SDL error: %s.", SDL_GetError ());
    }

    if (context != NULL) {
        SDL_GL_DeleteContext (context);
    }

    if (window != NULL) {
        SDL_DestroyWindow (window);
    }

    SDL_Quit ();
    IMG_Quit ();

    if (error) {
        logi ("Error # %d.", error);
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
