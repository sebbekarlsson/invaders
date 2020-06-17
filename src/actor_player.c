#include "include/actor_player.h"
#include "include/actor_bullet.h"
#include "include/main_scene.h"
#include <cengine/physics.h>
#include <cengine/application.h>
#include <cengine/keyboard.h>
#include <cengine/geometry.h>


extern texture_T* TEXTURE_SHEET;
extern unsigned int TEXTURE_HEART_ID;
extern application_T* APP;

actor_player_T* init_actor_player(float x, float y, float z)
{
    actor_player_T* actor_player = calloc(1, sizeof(struct ACTOR_PLAYER_STRUCT));
    actor_ship_T* ship = actor_ship_constructor((actor_ship_T*) actor_player, x, y, z, 100.0f);

    actor_T* actor = (actor_T*) actor_player;
    actor->friction = 2.5f;

    actor->tick = actor_player_tick;

    actor->sprite = init_sprite_with_texture(init_texture(texture_get_cut_id(
        TEXTURE_SHEET, 0, 0, 32, 32
    ), (void*)0, 32, 32));

    ship->on_death = actor_player_on_death;
    ship->protection = 0.6f;

    return actor_player;
}

void actor_player_on_death(actor_T* self)
{
    printf("Player died!\n");
}

static void move(actor_T* self, float mx, float my)
{
    if (mx != 0 && my != 0)
    {
        move(self, mx, 0);
        move(self, 0, my);
        return;
    }

    if (self->x + mx <= 0)
    {
        self->dx = 0;
        return;
    }

    if (self->x + mx + 32 >= RES_WIDTH)
    {
        self->dx = 0;
        return;
    }

    self->x += mx;
    self->y += my;
}

static actor_ship_T* _bullet_check_collision(actor_T* self, float mx, float my)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    const int W = 32;
    const int H = 32;

    for (int i = 0; i < main_scene->enemies->size; i++)
    {
        actor_T* a = main_scene->enemies->items[i];
        actor_ship_T* ship = (actor_ship_T*) a;

        float bx = a->x;
        float by = a->y;

        if (rect_intersects(
            (float[4]){self->x+mx, self->x+W+mx, self->y, self->y+H},
            (float[4]){bx, bx+W, by, by+H}
        ))
        {
            self->dx = 0;
            return ship;
        }

        if (rect_intersects(
            (float[4]){self->x, self->x+W, self->y+my, self->y+H+my},
            (float[4]){bx, bx+W, by, by+H}
        ))
        {
            self->dy = 0;
            return ship;
        }
    }

    return (void*)0;
}

void actor_player_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    actor_ship_T* ship = (actor_ship_T*) self;

    if (ship->died_at != 0)
    {
        double current_time = application_get_time(APP);

        if ((current_time - ship->died_at) >= 3)
        {
            main_scene_clear(main_scene);
            main_scene_setup(main_scene);
            return;
        }
    }
    
    actor_ship_tick(self);

    float move_force = 600.0f * APP->delta_time;

    physics_to_zero(&self->dx, self->friction);
    physics_to_zero(&self->dy, self->friction);

    if (keyboard_check_pressed(GLFW_KEY_LEFT))
    {
        physics_vec2_push(&self->dx, &self->dy, 180, move_force);
    }

    if (keyboard_check_pressed(GLFW_KEY_RIGHT))
    {
        physics_vec2_push(&self->dx, &self->dy, 0, move_force);
    }

    if (keyboard_check_pressed_once(GLFW_KEY_SPACE))
    {
        actor_bullet_T* bullet = init_actor_bullet(self->x, self->y, self->z, _bullet_check_collision);

        bullet->r = 0;
        bullet->g = 255;
        bullet->b = 0;
        
        actor_bullet_push((actor_T*)bullet, 90, 700.0f); 

        scene_add_actor(scene, (actor_T*) bullet);
    }

    move(self, self->dx * APP->delta_time, self->dy * APP->delta_time);
}
