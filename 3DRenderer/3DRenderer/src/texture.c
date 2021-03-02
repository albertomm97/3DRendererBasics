#include "texture.h"

tex2_t tex2_clone(tex2_t* t, tex2_t* new) {
    new->u = t->u;
    new->v = t->v;
}