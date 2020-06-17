#ifndef INVADERS_ACTOR_BULLET_H
#define INVADERS_ACTOR_BULLET_H
#include <cengine/actor.h>
#include "actor_ship.h"

typedef actor_ship_T* (*fcheck_collision)(actor_T* self, float mx, float my);

typedef struct ACTOR_BULLET_STRUCT
{
    actor_T base;
    float direction;
    fcheck_collision check_collision;
    float damage;
    float r;
    float g;
    float b;
} actor_bullet_T;

actor_bullet_T* init_actor_bullet(float x, float y, float z, fcheck_collision check_collision);

void actor_bullet_tick(actor_T* self);

void actor_bullet_push(actor_T* self, float direction, float force);
#endif
