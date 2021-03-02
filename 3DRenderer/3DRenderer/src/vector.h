#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    float x, y, z, w;
} vec4_t;

////////////////////////////////////////////////////////////////////////////////
// Vector 2 functions
////////////////////////////////////////////////////////////////////////////////
void vec2_new(float x, float y, vec2_t* new);
float vec2_length(vec2_t* v);
void vec2_add(vec2_t* a, vec2_t* b, vec2_t* r);
void vec2_sub(vec2_t* a, vec2_t* b, vec2_t* r);
void vec2_mul(vec2_t* v, float factor);
void vec2_div(vec2_t* v, float factor);
void vec2_normalize(vec2_t* v);

////////////////////////////////////////////////////////////////////////////////
// Vector 3 functions
////////////////////////////////////////////////////////////////////////////////
void vec3_new(float x, float y, float z, vec3_t* new);
void vec3_clone(vec3_t* v, vec3_t* out);
float vec3_length(vec3_t* v);
void vec3_add(vec3_t* a, vec3_t* b, vec3_t* r);
void vec3_sub(vec3_t* a, vec3_t* b, vec3_t* r);
void vec3_mul(vec3_t* v, float factor);
void vec3_div(vec3_t* v, float factor);
void vec3_cross(vec3_t* a, vec3_t* b, vec3_t* r);
float vec3_dot(vec3_t* a, vec3_t* b);
void vec3_normalize(vec3_t* v);
void vec3_rotate_x(vec3_t* v, float angle);
void vec3_rotate_y(vec3_t* v, float angle);
void vec3_rotate_z(vec3_t* v, float angle);

////////////////////////////////////////////////////////////////////////////////
// Vector conversion functions
////////////////////////////////////////////////////////////////////////////////
void vec4_from_vec3(vec3_t* v, vec4_t* new);
void vec3_from_vec4(vec4_t* v, vec3_t* new);
void vec2_from_vec4(vec4_t* v, vec2_t* new);
#endif
