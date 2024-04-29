#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "macro_consts.h"
#include "glad/include/glad/glad.h"
#include "vec2.h"
#include "color4f.h"

#include "coins.h"
#include "error_codes.h"

/* struct _coin {
  vec2 bpos, pos;
  float rad;
  color4f color;
  bool exist;
}; */

static float _ratio_ = 1;

coin coin_init(float x, float y, float rad, float r, float g, float b)
{
  coin res;
  
  res.bpos = vec2_init(x,y);
  res.pos = res.bpos;
  res.rad = rad;
  res.color = _color4f_(r,g,b,1.0f);
  res.exist = true;
  res.quality = COINQUALITY;

  return res;
}

void setRatio(float rat) {
  _ratio_ = rat;
}

void setQuality(coin *cn, int q) {
  cn->quality = q;
}

void coin_draw(coin* cn)
{       
  if (cn->exist) {
    float x, y;
    const float L = 0.5f;
    float a;
  
    
    float vertex_arr[cn->quality+1][2];
    float color_arr[cn->quality+1][4];

    glPushMatrix();
    glTranslatef(cn->pos.x,cn->pos.y,0.0f);
    glScalef(cn->rad / _ratio_,cn->rad,1);

    a = 3.14 * 2 / cn->quality;

    for (int i = -1; i <= cn->quality; ++i) {
      x = sin(a * i) * L;
      y = cos(a * i) * L;
      vertex_arr[i][0] = x;
      vertex_arr[i][1] = y;

      color_arr[i][0] = cn->color.r;
      color_arr[i][1] = cn->color.g;
      color_arr[i][2] = cn->color.b;
      color_arr[i][3] = cn->color.a;
    }
      
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2,GL_FLOAT,NULL,vertex_arr);
    glColorPointer(4,GL_FLOAT,NULL,color_arr);

    glDrawArrays(GL_TRIANGLE_FAN, NULL, cn->quality+1);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }
}

void coin_moveon(coin* cn, vec2* vec) {
  cn->pos = vec2_plus(cn->pos, *vec);
}
void coin_moveto(coin* cn, vec2* pos) {
  cn->pos = *pos;
}
