mat4
mat4_identity (void) {
    mat4 out = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }};

    return out;
}

mat4
mat4_inverted_rtonly (
        mat4 const * m
) {
    mat4 out;

    out.p[0] = m->p[0];
    out.p[5] = m->p[5];
    out.p[10] = m->p[10];

    out.p[4] = m->p[1];
    out.p[8] = m->p[2];
    out.p[9] = m->p[6];

    out.p[1] = m->p[4];
    out.p[2] = m->p[8];
    out.p[6] = m->p[9];

    float x = -m->p[12];
    float y = -m->p[13];
    float z = -m->p[14];

    out.p[12] = (out.p[0]) * x + (out.p[4]) * y + (out.p[ 8]) * z;
    out.p[13] = (out.p[1]) * x + (out.p[5]) * y + (out.p[ 9]) * z;
    out.p[14] = (out.p[2]) * x + (out.p[6]) * y + (out.p[10]) * z;

    out.p[3] = 0.0f;
    out.p[7] = 0.0f;
    out.p[11] = 0.0f;
    out.p[15] = 1.0f;

    return out;
}

mat4
mat4_moved (
        mat4 const * m,
        vec3 const * v
) {
	mat4 mtra = mat4_identity ();
    mtra.column.w.v3 = *v;
	return mat4_multiply (m, & mtra);
}

mat4
mat4_multiply (
        mat4 const * a,
        mat4 const * b
) {
    mat4 out;

    for (int i = 0; i < 4; ++i) { /* columns */
        for (int j = 0; j < 4; ++j) { /* rows */
            float * s = & out.p[i * 4 + j];
            *s = 0.0f;
            for (int k = 0; k < 4; ++k) {
                *s += a->p[k * 4 + j] * b->p[i * 4 + k];
            }
        }
    }

    return out;
}

void
mat4_print (
        mat4 const * m
) {
    for (unsigned i = 0; i < 4; ++i) {
        log_info ("\t( %.3f\t%.3f\t%.3f\t%.3f )",
                m->p[i + 0], m->p[i + 4],
                m->p[i + 8], m->p[i + 12]);
    }
}

mat4
mat4_rotated_aa (
        mat4 const * m,
        vec3 const * axis,
        float angle
) {
    float len = vec3_length (axis);
    assert (len != 0.0f);
   
    float x = axis->p[0] / len;
    float y = axis->p[1] / len;
    float z = axis->p[2] / len;

	float c = cosf (angle);
	float s = sinf (angle);

	float cc = 1.0f - c;
	float xs = x * s;
	float ys = y * s;
	float zs = z * s;

	mat4 mrot = {{
		x*x * cc + c,
		y*x * cc + zs,
		z*x * cc - ys,
		0.0f,

		x*y * cc - zs,
		y*y * cc + c,
		z*y * cc + xs,
		0.0f,

		x*z * cc + ys,
		y*z * cc - xs,
		z*z * cc + c,
		0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f
	}};

	return mat4_multiply (m, & mrot);
}

mat4
mat4_scaled (
        mat4 const * m,
        float scale
) {
    mat4 out = * m;

	for (int n = 0; n < 12; ++n) {
		out.p[n] *= scale;
        /* what about the 'translation' elements? */
	}

    return out;
}

vec3
vec3_diff (
		vec3 const * a,
		vec3 const * b
) {
    vec3 out;

	out.p[0] = a->p[0] - b->p[0];
	out.p[1] = a->p[1] - b->p[1];
	out.p[2] = a->p[2] - b->p[2];

    return out;
}

float
vec3_dot (
		vec3 const * a,
		vec3 const * b
) {
    return
        (a->p[0] * b->p[0]) +
        (a->p[1] * b->p[1]) +
        (a->p[2] * b->p[2]);
}

float
vec3_length (
        vec3 const * v
) {
	return sqrtf (vec3_dot (v, v));
}

vec3
vec3_normalized (
        vec3 const * v
) {
	float len = vec3_length (v);
    assert (len != 0.0f);
    return vec3_scaled (v, 1.0f / len);
}

vec3
vec3_product (
		vec3 const * a,
		vec3 const * b
) {
    vec3 out;

	out.p[0] = a->p[1] * b->p[2] - a->p[2] * b->p[1];
	out.p[1] = a->p[2] * b->p[0] - a->p[0] * b->p[2];
	out.p[2] = a->p[0] * b->p[1] - a->p[1] * b->p[0];

    return out;
}

vec3
vec3_scaled (
        vec3 const * v,
        float scale
) {
    vec3 out;

	out.p[0] = v->p[0] * scale;
	out.p[1] = v->p[1] * scale;
	out.p[2] = v->p[2] * scale;

    return out;
}

vec3
vec3_sum (
		vec3 const * a,
		vec3 const * b
) {
    vec3 out;

	out.p[0] = a->p[0] + b->p[0];
	out.p[1] = a->p[1] + b->p[1];
	out.p[2] = a->p[2] + b->p[2];

    return out;
}

vec4
vec4_multiply (
        mat4 const * m,
        vec4 const * v
) {
    vec4 out = {0};

    for (int j = 0; j < 4; ++j) { /* rows */
        for (int k = 0; k < 4; ++k) {
            out.p[j] += m->p[k * 4 + j] * v->p[k];
        }
    }

    return out;
}