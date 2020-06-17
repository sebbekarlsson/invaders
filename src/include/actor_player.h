#ifndef INVADERS_ACTOR_PLAYER_H
#define INVADERS_ACTOR_PLAYER_H
#include "actor_ship.h"
typedef struct ACTOR_PLAYER_STRUCT
{
    actor_ship_T base;
} actor_player_T;

actor_player_T* init_actor_player(float x, float y, float z);

void actor_player_tick(actor_T* self);

void actor_player_on_death(actor_T* self);
#endif
