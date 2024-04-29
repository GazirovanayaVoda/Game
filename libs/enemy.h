#ifndef ENEMY_H
#define ENEMY_H

#include "color4f.h"
#include"vec2.h"
#include <stdbool.h>
#include"glad/include/glad/glad.h"

typedef struct _enemy_t {
  vec2 bpos, pos, size, speed;
  color4f color;
  bool alive;
  int8_t hp, damage;
} enemy;

enemy enemy_init(float, float, float, float, float, float, float, float, float, float, int8_t);

void enemy_setratio(float);

void enemy_draw(enemy*);

void enemy_moveon(enemy*, vec2*);
void enemy_moveto(enemy*, vec2*);

void enemy_update(vec2*, enemy*, float);
bool enemy_giveDamage(vec2*, enemy*, int8_t*);
void enemy_reset_pos(enemy *);


#endif