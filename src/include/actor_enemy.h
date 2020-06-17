#ifndef INVADERS_ACTOR_ENEMY_H
#define INVADERS_ACTOR_ENEMY_H
#include "actor_ship.h"
typedef struct ACTOR_ENEMY_STRUCT
{
    actor_ship_T base;
    float target_x;
    float target_y;
    float target_error;
} actor_enemy_T;

actor_enemy_T* init_actor_enemy(float x, float y, float z);

void actor_enemy_tick(actor_T* self);

void actor_enemy_on_death(actor_T* self);
#endif
