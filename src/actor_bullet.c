#include "include/actor_bullet.h"
#include <cengine/application.h>
#include <cengine/physics.h>


extern application_T* APP;
extern texture_T* TEXTURE_SHEET;

actor_bullet_T* init_actor_bullet(float x, float y, float z, fcheck_collision check_collision)
{
    actor_bullet_T* actor_bullet = calloc(1, sizeof(struct ACTOR_BULLET_STRUCT));
    actor_T* actor = actor_constructor((actor_T*) actor_bullet, x, y, z);

    actor->tick = actor_bullet_tick;

    actor->sprite = init_sprite_with_texture(init_texture(texture_get_cut_id(
        TEXTURE_SHEET, 32, 0, 32, 32
    ), (void*)0, 32, 32));

    actor->friction = 0.0f;

    actor_bullet->direction = 0;
    actor_bullet->check_collision = check_collision;

    actor_bullet->damage = 10.0f;
    actor_bullet->r = 255;
    actor_bullet->g = 255;
    actor_bullet->b = 255;

    return actor_bullet;
}

static void move(actor_T* self, float mx, float my)
{
    scene_T* scene = application_get_current_scene(APP);

    if (mx != 0 && my != 0)
    {
        move(self, mx, 0);
        move(self, 0, my);
        return;
    }
    
    actor_bullet_T* bullet = (actor_bullet_T*) self;

    if (bullet->check_collision)
    {
        actor_ship_T* ship = bullet->check_collision(self, mx, my);

        if (ship != (void*)0)
        {
            actor_ship_damage(ship, bullet->damage - (bullet->damage * ship->protection));

            printf("Bullet hit with %p! Removing bullet\n", ship);
            scene_remove_actor(scene, self);
            return;
        }
    }

    if ((self->y+32) + my < 0 || self->y + my > RES_HEIGHT)
    {
        printf("Bullet outside view, removing!\n");
        scene_remove_actor(scene, self);
        return;
    }

    self->x += mx;
    self->y += my;
}

void actor_bullet_tick(actor_T* self)
{
    actor_bullet_T* bullet = (actor_bullet_T*) self;
    self->sprite->r = bullet->r;
    self->sprite->g = bullet->g;
    self->sprite->b = bullet->b;

    physics_to_zero(&self->dx, self->friction);
    physics_to_zero(&self->dy, self->friction);


    move(self, self->dx * APP->delta_time, self->dy * APP->delta_time);
}

void actor_bullet_push(actor_T* self, float direction, float force)
{
    actor_bullet_T* bullet = (actor_bullet_T*) self;
    bullet->direction = direction;
    physics_vec2_push(&self->dx, &self->dy, bullet->direction, force);
}
