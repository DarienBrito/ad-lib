function DrawSphere (S)
    Sphere (S.tMat, S.colour, S.radius)
end

function DrawSpheres ()
    PreSphere ()
    apply (DrawSphere, World.spheres)
end

function PreSphere ()
    GL.DepthMask (GL.TRUE)
    GL.Enable (GL.DEPTH_TEST)
    GL.Disable (GL.BLEND)
    GL.Disable (GL.STENCIL_TEST)

    GL.UseProgram (GPlanet.glsl.program)
    GL.BindBuffer (GL.ARRAY_BUFFER, VImposter.vbo)
    -- these two guys need to be called after GL.BindBuffer
	local Apos2d = GPlanet.attribute.Apos2d
    GL.VertexAttribPointer (Apos2d, 2, GL.FLOAT, GL.FALSE, 0, NULL)
    GL.EnableVertexAttribArray (Apos2d)

    -- kind of hacky...
    GL.ActiveTexture (GL.TEXTURE0)
end

function Sphere (tMat, colour, radius)
    --[[ this is somewhat hacky and might need more work
     * we want to split tMat into pure translation and rotation
     * for some unknown yet reason I had to invert the rotation
     * so it looked right. Well, that's it! --]]
    local mModel = mat4.Copy (tMat)
    mModel.c.x = vec4 (1,0,0,0)
    mModel.c.y = vec4 (0,1,0,0)
    mModel.c.z = vec4 (0,0,1,0)
	local mRot = mat4.Copy (tMat)
    mRot.c.w = vec4.zero
    mRot = mat4.Inverse (mRot)

	local first = mModel.c.w.v3 - Sviewi.c.w.v3
    local p = vec3.Length (first)
    local r = radius
    local apparent = math.sqrt (p * p - r * r) * r / p
    local offset = (r * r) / p

    local second = first.e.x < first.e.y and
        core.vec3_product (first, vec3.x) or
        core.vec3_product (first, vec3.y)

    local third = core.vec3_product (first, second)

    local rotation = mat4.Copy (mat4.id)
    rotation.c.z.v3 = vec3.Normalized (first)
    rotation.c.x.v3 = vec3.Normalized (second)
    rotation.c.y.v3 = vec3.Normalized (third)
    local mFinalModel = mModel ^ rotation
	local mFinalRot = mRot ^ mFinalModel

	local mSuperModel = mat4.Moved (mFinalModel, vec3 (0, 0, -offset))
	local mvp = Sviewproj ^ mSuperModel

    local uniform = GPlanet.uniform

    GL.UniformMatrix4fv (uniform.Umvp, 1, GL.FALSE, mvp.p)
    GL.UniformMatrix4fv (uniform.Umv, 1, GL.FALSE, mFinalRot.p)
    GL.Uniform1f (uniform.Uuvscale, apparent / r)
    GL.Uniform1f (uniform.UR, r)
    GL.Uniform1i (uniform.Utexture, Tex.tex)
    GL.Uniform3fv (uniform.Ucolour, 1, colour.p)

    GL.DrawArrays (GL.TRIANGLES, 0, VImposter.size)
end

function VImposterInit ()
	local function gc (V)
    	GL.DeleteBuffers (1, V.vbo);
	end
	VImposter = FFI.gc (core.util_imposter (), gc)
end

function GPlanetInit ()
	local function hot (null, file, text)
		GPlanet = LoadShader (file, text)
	end
	core.Pull ("glsl/planet.glts", hot)
end

function TexInit ()
	local function gc (T)
		GL.DeleteTextures (1, T.tex);
	end
    Tex = FFI.gc (core.util_earth (), gc)
end