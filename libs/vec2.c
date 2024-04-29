#include "vec2.h"

vec2 vec2_init(float x, float y) {
    return (vec2){x,y};
};

/* vec2 + vec2 */
vec2 vec2_plus(vec2 a, vec2 b) {
    return (vec2){a.x + b.x, a.y + b.y};
};
