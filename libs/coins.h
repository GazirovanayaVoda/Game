#ifndef COINS_INCLUDED
#define COINS_INCLUDED

#include <stdbool.h>
#include "glad/include/glad/glad.h"
#include "vec2.h"
#include "color4f.h"

typedef struct _coin {
  vec2 bpos, pos;
  float rad;
  color4f color;
  bool exist;
  int quality;
} coin;

coin coin_init(float, float, float, float, float, float);

//void init_coins(const uint8_t, coin*, const char**);

void setRatio(float);
void setQuality(coin *, int); 

void coin_draw(coin*);

void coin_moveon(coin*, vec2*);
void coin_moveto(coin*, vec2*);

#endif
