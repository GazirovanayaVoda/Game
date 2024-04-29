#ifndef COLOR4F_INCLUDED
#define COLOR4F_INCLUDED

#include<stdbool.h>
/* color4f type */

struct clr4f {
  float r,g,b,a;
};

typedef struct clr4f color4f;

/* color4f initializer */
color4f _color4f_(float, float, float, float);

#endif
