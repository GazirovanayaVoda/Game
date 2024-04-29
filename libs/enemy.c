#include"enemy.h"
#include "color4f.h"
#include"vec2.h"
#include <stdbool.h>
#include"glad/include/glad/glad.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "macro_consts.h"

#include "coins.h"
#include "error_codes.h"

static float _ratio_ = 1;
void enemy_setratio(float rat)
{
  _ratio_ = rat;
}


enemy enemy_init(float x, float y, float sizex, float sizey, float spdx, float spdy, float r, float g, float b, float a, int8_t dmg)
{
  enemy result;

  result.bpos = vec2_init(x, y);
  result.pos = result.bpos;
  result.size = vec2_init(sizex, sizey);
  result.speed = vec2_init(spdx, spdy);
  result.color = _color4f_(r,g,b,a);
  result.damage = dmg;
  result.hp = INT8MAX;
  result.alive = true;

  return result;
}

void enemy_draw(enemy* enem)
{
  if (enem->alive) {
    float vertex_arr[4][2] = {
      {0.0f,0.0f},
      {1.0f,0.0f},
      {0.0f,1.0f},
      {1.0f,1.0f},
      };
      
    float horns_ver[6][2] = {
      {0.0f,1.0f},  /*Left*/
      {0.5f,1.0f},
      {0.2f,1.4f},

      {0.5f,1.0f},  /*Right*/
      {1.0f,1.0f},
      {0.8f,1.4f}
    };

    float color_arr[10][4];
    for (uint8_t i = 0; i < 10; ++i) {
      color_arr[i][0] = enem->color.r;
      color_arr[i][1] = enem->color.g;
      color_arr[i][2] = enem->color.b;
      color_arr[i][3] = enem->color.a;
    }

    glPushMatrix();
    glTranslatef(enem->pos.x, enem->pos.y, .0f);
    glScalef(enem->size.x/_ratio_, enem->size.y, .0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2,GL_FLOAT,NULL,vertex_arr); /*2 - for 2d; 3 - for 3d*/
    glColorPointer(4,GL_FLOAT,NULL,color_arr);  /*3 - for RGB; 4 - for RGBA*/

    glDrawArrays(GL_TRIANGLE_STRIP, NULL, 4);

    glVertexPointer(2,GL_FLOAT,NULL,horns_ver);
    glDrawArrays(GL_TRIANGLES, NULL, 6);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
  }
}


void enemy_moveon(enemy* enem, vec2* vec)
{
  enem->pos = vec2_plus(enem->pos, *vec);
}
void enemy_moveto(enemy* enem, vec2* pos)
{
  enem->pos = *pos;
}

void enemy_update(vec2* pl_pos, enemy* enem, float fps_mult)
{
  vec2 fin_speed;
  if ((pl_pos->x * _ratio_) < enem->pos.x) {
    fin_speed.x = -enem->speed.x * fps_mult;
  } else if ((pl_pos->x * _ratio_) > enem->pos.x){
    fin_speed.x = enem->speed.x * fps_mult;
  } else {
    fin_speed.x = 0;
  } 

  fin_speed.x = fin_speed.x / _ratio_;

  if (pl_pos->y < enem->pos.y) {
    fin_speed.y = -enem->speed.y * fps_mult;
  } else if (pl_pos->y > enem->pos.y) {
    fin_speed.y = enem->speed.y * fps_mult;
  } else {
    fin_speed.y = 0;
  }

  enemy_moveon(enem, &fin_speed);

  if (enem->hp < 0) {
    enem->alive = false;
  }
}

bool enemy_giveDamage(vec2* mouse_pos, enemy* enem, int8_t* pldamage)
{
  bool mouse_in_enemy = (mouse_pos->x > enem->pos.x) &&
                        (mouse_pos->x < (enem->pos.x + enem->size.x)) &&
                        (mouse_pos->y > enem->pos.y) &&
                        (mouse_pos->y < enem->pos.y + enem->size.y);

  if (mouse_in_enemy && enem->alive) {

    printf("Enemy hit, hp: %d\n", enem->hp);
    enem->hp -= *pldamage;
    return true;
  }
  return false;
}

void enemy_reset_pos(enemy *enem)
{
  enemy_moveto(enem, &enem->bpos);
}