#ifndef VEC2_INCLUDED
#define VEC2_INCLUDED

typedef struct {
    float x,y;
} vec2;

vec2 vec2_init(float, float);
vec2 vec2_plus(vec2, vec2);

#endif
