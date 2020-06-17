#ifndef APP_MAIN_SCENE_H
#define APP_MAIN_SCENE_H
#include <cengine/scene.h>
#include "actor_player.h"


#define LOSE_TEXT "You Died!"

typedef struct MAIN_SCENE_STRUCT
{
    scene_T base;
    dynamic_list_T* enemies;
    actor_player_T* player;
    int difficulty;
    float bg_shift;
} main_scene_T;

main_scene_T* init_main_scene();

void main_scene_setup(main_scene_T* main_scene);

void main_scene_clear(main_scene_T* main_scene);

void main_scene_predraw(scene_T* self);

void main_scene_draw(scene_T* self);

void main_scene_spawn_enemy(main_scene_T* main_scene);

void main_scene_tick(scene_T* self);

void main_scene_advance(main_scene_T* main_scene);
#endif
