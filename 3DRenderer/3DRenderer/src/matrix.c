#include <math.h>
#include <stdlib.h>
#include "matrix.h"

void mat4_identity(mat4_t* m) 
{
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            if (i == j)
                m->m[i][j] = 1;
            else
                m->m[i][j] = 0;
        }
    }
}

void mat4_make_scale(float sx, float sy, float sz, mat4_t* scaled) 
{
    // | sx  0  0  0 |
    // |  0 sy  0  0 |
    // |  0  0 sz  0 |
    // |  0  0  0  1 |
    mat4_identity(scaled);
    scaled->m[0][0] = sx;
    scaled->m[1][1] = sy;
    scaled->m[2][2] = sz;
}

void mat4_make_translation(float tx, float ty, float tz, mat4_t* trans) 
{
    // | 1  0  0  tx |
    // | 0  1  0  ty |
    // | 0  0  1  tz |
    // | 0  0  0  1  |
    mat4_identity(trans);
    trans->m[0][3] = tx;
    trans->m[1][3] = ty;
    trans->m[2][3] = tz;
}

void mat4_make_rotation_x(mat4_t* rotate, float angle) 
{
    float c = cos(angle);
    float s = sin(angle);
    // | 1  0  0  0 |
    // | 0  c -s  0 |
    // | 0  s  c  0 |
    // | 0  0  0  1 |
    mat4_identity(rotate);
    rotate->m[1][1] = c;
    rotate->m[1][2] = -s;
    rotate->m[2][1] = s;
    rotate->m[2][2] = c;
}

void mat4_make_rotation_y(mat4_t* rotate, float angle) 
{
    float c = cos(angle);
    float s = sin(angle);
    // |  c  0  s  0 |
    // |  0  1  0  0 |
    // | -s  0  c  0 |
    // |  0  0  0  1 |
    mat4_identity(rotate);
    rotate->m[0][0] = c;
    rotate->m[0][2] = s;
    rotate->m[2][0] = -s;
    rotate->m[2][2] = c;
}

void mat4_make_rotation_z(mat4_t* rotate, float angle) 
{
    float c = cos(angle);
    float s = sin(angle);
    // | c -s  0  0 |
    // | s  c  0  0 |
    // | 0  0  1  0 |
    // | 0  0  0  1 |
    mat4_identity(rotate);
    rotate->m[0][0] = c;
    rotate->m[0][1] = -s;
    rotate->m[1][0] = s;
    rotate->m[1][1] = c;
}

void mat4_mul_vec4(mat4_t* m, vec4_t* v, vec4_t* result) 
{
    result->x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w;
    result->y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w;
    result->z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w;
    result->w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w;
}

void mat4_mul_mat4(mat4_t* a, mat4_t* b, mat4_t* m) 
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m->m[i][j] = a->m[i][0] * b->m[0][j] + a->m[i][1] * b->m[1][j] + a->m[i][2] * b->m[2][j] + a->m[i][3] * b->m[3][j];
        }
    }
}

void mat4_make_perspective(float fov, float aspect, float znear, float zfar, mat4_t* r) 
{
    // | (h/w)*1/tan(fov/2)             0              0                 0 |
    // |                  0  1/tan(fov/2)              0                 0 |
    // |                  0             0     zf/(zf-zn)  (-zf*zn)/(zf-zn) |
    // |                  0             0              1                 0 |
    for (int i = 0; i < 4; i++)
        memset(r->m[i][0], 0, sizeof(float) * 4);

    r->m[0][0] = aspect * (1 / tan(fov / 2));
    r->m[1][1] = 1 / tan(fov / 2);
    r->m[2][2] = zfar / (zfar - znear);
    r->m[2][3] = (-zfar * znear) / (zfar - znear);
    r->m[3][2] = 1.0;
}

void mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up, mat4_t* v) 
{
    // Compute the forward (z), right (x), and up (y) vectors
    vec3_sub(target, eye, target);
    vec3_normalize(&target);
    vec3_t x = { {0} };
    vec3_cross(up, target, &x);
    vec3_normalize(&x);
    vec3_t y = { {0} };
    vec3_cross(target, &x, &y);

    // | x.x   x.y   x.z  -dot(x,eye) |
    // | y.x   y.y   y.z  -dot(y,eye) |
    // | z.x   z.y   z.z  -dot(z,eye) |
    // |   0     0     0            1 |
    v->m[0][0] = x.x;
    v->m[0][1] = x.y; 
    v->m[0][2] = x.z;
    v->m[0][3] = -vec3_dot(&x, eye);
    v->m[1][0] = y.x;
    v->m[1][1] = y.y;
    v->m[1][2] = y.z;
    v->m[1][3] = -vec3_dot(&y, eye);
    v->m[2][0] = target->x;
    v->m[2][1] = target->y;
    v->m[2][2] = target->z;
    v->m[2][3] = -vec3_dot(target, eye);
    v->m[3][0] = 0;
    v->m[3][1] = 0;
    v->m[3][2] = 0;
    v->m[3][3] = 1;
}
