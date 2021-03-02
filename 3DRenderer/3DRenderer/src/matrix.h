#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} mat4_t;

void mat4_identity(mat4_t* m);
void mat4_make_scale(float sx, float sy, float sz, mat4_t* scaled);
void mat4_make_translation(float tx, float ty, float tz, mat4_t* trans);
void mat4_make_rotation_x(mat4_t* rotate, float angle);
void mat4_make_rotation_y(mat4_t* rotate, float angle);
void mat4_make_rotation_z(mat4_t* rotate, float angle);
void mat4_mul_vec4(mat4_t* m, vec4_t* v, vec4_t* result);
void mat4_mul_mat4(mat4_t* a, mat4_t* b, mat4_t* m);
void mat4_make_perspective(float fov, float aspect, float znear, float zfar, mat4_t* r);
void mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up, mat4_t* v);

#endif
