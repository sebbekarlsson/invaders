#ifndef INVADERS_ACTOR_SHIP_H
#define INVADERS_ACTOR_SHIP_H
#include <cengine/actor.h>
typedef struct ACTOR_SHIP_STRUCT
{
    actor_T base;
    float health;
    float protection;
    void (*on_death)(actor_T* self);
    double died_at;
} actor_ship_T;

actor_ship_T* actor_ship_constructor(actor_ship_T* actor, float x, float y, float z, float health);

void actor_ship_draw(actor_T* self);

void actor_ship_damage(actor_ship_T* ship, float damage);

void actor_ship_tick(actor_T* self);
#endif
