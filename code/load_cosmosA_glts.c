struct cosmosA_glts
  load_cosmosA_glts (
            struct GL * gl,
            char const * filename
) {
    struct cosmosA_glts it;
    it.program = load_glts (gl, filename, "cosmosA", "");
    // we could check that program's not GL_FALSE, but we won't.
    it.layout.U.mv = glGetUniformLocation (it.program, "U.mv");
    it.layout.U.mvp = glGetUniformLocation (it.program, "U.mvp");
    it.layout.U.colour = glGetUniformLocation (it.program, "U.colour");
    it.layout.U.depth = glGetUniformLocation (it.program, "U.depth");
    it.layout.U.uvscale = glGetUniformLocation (it.program, "U.uvscale");
    it.layout.Utexture = glGetUniformLocation (it.program, "Utexture");
    // ignore all missing uniforms
    return it;
}
