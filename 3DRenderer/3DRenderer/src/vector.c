#include <math.h>
#include "vector.h"

///////////////////////////////////////////////////////////////////////////////
// Implementations of Vector 2 functions
///////////////////////////////////////////////////////////////////////////////
void vec2_new(float x, float y, vec2_t* new) 
{
    new->x = x;
    new->y = y;
}

float vec2_length(vec2_t* v) 
{
    return sqrt(v->x * v->x + v->y * v->y);
}

void vec2_add(vec2_t* a, vec2_t* b, vec2_t* r)
{
    r->x = a->x + b->x;
    r->y = a->y + b->y;
}

void vec2_sub(vec2_t* a, vec2_t* b, vec2_t* r)
{
    r->x = a->x - b->x;
    r->y = a->y - b->y;
}

void vec2_mul(vec2_t* v, float factor) 
{
    v->x *= factor;
    v->y *= factor;
}

void vec2_div(vec2_t* v, float factor) 
{
    v->x /= factor;
    v->y /= factor;
}

float vec2_dot(vec2_t* a, vec2_t* b) 
{
    return (a->x * b->x) + (a->y * b->y);
}

void vec2_normalize(vec2_t* v) 
{
    float length = sqrt(v->x * v->x + v->y * v->y);
    v->x /= length;
    v->y /= length;
}

///////////////////////////////////////////////////////////////////////////////
// Implementations of Vector 3 functions
///////////////////////////////////////////////////////////////////////////////
void vec3_new(float x, float y, float z, vec3_t* new) 
{
    new->x = x;
    new->y = y;
    new->z = z;
}

void vec3_clone(vec3_t* v, vec3_t* out) 
{
    out->x = v->x; out->y = v->y; out->z = v->z;
}

float vec3_length(vec3_t* v) 
{
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec3_add(vec3_t* a, vec3_t* b, vec3_t* r)
{
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
}

void vec3_sub(vec3_t* a, vec3_t* b, vec3_t* r) 
{
    r->x = a->x - b->x;
    r->y = a->y - b->y;
    r->z = a->z - b->z;
}

void vec3_mul(vec3_t* v, float factor)
{
    v->x *= factor;
    v->y *= factor;
    v->z *= factor;
}

void vec3_div(vec3_t* v, float factor) 
{
    v->x /= factor;
    v->y /= factor;
    v->z /= factor;
}

void vec3_cross(vec3_t* a, vec3_t* b, vec3_t* r)
{
    r->x = a->y * b->z - a->z * b->y;
    r->y = a->z * b->x - a->x * b->z;
    r->z = a->x * b->y - a->y * b->x;
}

float vec3_dot(vec3_t* a, vec3_t* b) 
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

void vec3_normalize(vec3_t* v) 
{
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

void vec3_rotate_x(vec3_t* v, float angle) 
{
    v->x = v->x;
    v->y = v->y * cos(angle) - v->z * sin(angle);
    v->z = v->y * sin(angle) + v->z * cos(angle);
}

void vec3_rotate_y(vec3_t* v, float angle) 
{
    v->x = v->x * cos(angle) + v->z * sin(angle);
    v->y = v->y;
    v->z = -v->x * sin(angle) + v->z * cos(angle);
}

void vec3_rotate_z(vec3_t* v, float angle) 
{
    v->x = v->x * cos(angle) - v->y * sin(angle);
    v->y = v->x * sin(angle) + v->y * cos(angle);
    v->z = v->z;
}

///////////////////////////////////////////////////////////////////////////////
// Implementations of Vector conversion functions
///////////////////////////////////////////////////////////////////////////////
void vec4_from_vec3(vec3_t* v, vec4_t* new)
{
    new->x = v->x; new->y = v->y; new->z = v->z; new->w = 1.0;
}

void vec3_from_vec4(vec4_t* v, vec3_t* new) 
{
    new->x = v->x; new->y = v->y; new->z = v->z;
}

void vec2_from_vec4(vec4_t* v, vec2_t* new) 
{
    new->x = v->x; new->y = v->y;
}
