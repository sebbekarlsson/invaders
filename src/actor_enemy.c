#include "include/actor_enemy.h"
#include "include/actor_bullet.h"
#include "include/main_scene.h"
#include <cengine/random.h>
#include <cengine/physics.h>
#include <cengine/application.h>
#include <cengine/keyboard.h>
#include <cengine/geometry.h>
#include <sys/param.h>


extern texture_T* TEXTURE_SHEET;
extern application_T* APP;

actor_enemy_T* init_actor_enemy(float x, float y, float z)
{
    actor_enemy_T* actor_enemy = calloc(1, sizeof(struct ACTOR_ENEMY_STRUCT));
    actor_ship_T* ship = actor_ship_constructor((actor_ship_T*) actor_enemy, x, y, z, 100.0f);

    actor_T* actor = (actor_T*) actor_enemy;
    actor->friction = 2.5f;

    actor->tick = actor_enemy_tick;

    actor->sprite = init_sprite_with_texture(init_texture(texture_get_cut_id(
        TEXTURE_SHEET, 32 * 2, 0, 32, 32
    ), (void*)0, 32, 32));
    actor->sprite->flip_y = 1;

    actor_enemy->target_x = x;
    actor_enemy->target_y = get_random_int(0, 6) * 32;
    actor_enemy->target_error = 0;

    ship->on_death = actor_enemy_on_death;

    return actor_enemy;
}

static void _actor_ship_free(void* x)
{
    actor_T* self = (actor_T*) x;
    actor_free(self);
}

void actor_enemy_on_death(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    printf("Enemy died!\n");

    main_scene_advance(main_scene);

    dynamic_list_remove(main_scene->enemies, self, (void*)0);
    dynamic_list_remove(scene->actors, self, _actor_ship_free);
}

static void move(actor_T* self, float mx, float my)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;
    actor_enemy_T* enemy = (actor_enemy_T*) self;

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

    const int W = 32;
    const int H = 32;

    for (int i = 0; i < main_scene->enemies->size; i++)
    {
        actor_T* a = main_scene->enemies->items[i];
        actor_ship_T* ship = (actor_ship_T*) a;

        if (self == a)
            continue;

        float bx = a->x;
        float by = a->y;

        if (rect_intersects(
            (float[4]){self->x+mx, self->x+W+mx, self->y, self->y+H},
            (float[4]){bx, bx+W, by, by+H}
        ))
        {
            self->dx = 0;
            return;
        }

        if (rect_intersects(
            (float[4]){self->x, self->x+W, self->y+my, self->y+H+my},
            (float[4]){bx, bx+W, by, by+H}
        ))
        {
            self->dy = 0;
            return;
        }
    }

    if (self->y + my < enemy->target_y)
    {
        float move_speed = 2.0f;
        physics_vec2_push(&self->dx, &self->dy, 270, move_speed);
    }
    else
    {
        self->dy = 0;
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

    actor_T* a = (actor_T*) main_scene->player;
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

    return (void*)0;
}

void actor_enemy_tick(actor_T* self)
{
    //printf("%12.6f, %12.6f\n", self->x, self->y);
    actor_enemy_T* enemy = (actor_enemy_T*) self;
    actor_ship_T* ship = (actor_ship_T*) self;
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;

    actor_ship_tick(self);

    //float move_force = 30.0f;

    physics_to_zero(&self->dx, self->friction);
    physics_to_zero(&self->dy, self->friction);

    float px = ((actor_T*)main_scene->player)->x;
    enemy->target_x = (px + enemy->target_error);

    if (get_random_int(0, 20) == 0)
    {
        enemy->target_error = get_random_int(0, 32 * 6);

        if (get_random_int(0, 1) == 0)
            enemy->target_error = enemy->target_error * -1;
    }

    enemy->target_x = MAX(0, MIN(RES_WIDTH-32, enemy->target_x));

    float move_speed = 600.0f * APP->delta_time;

    if (self->x < enemy->target_x)
    {
        physics_vec2_push(&self->dx, &self->dy, 0, move_speed);
    }

    if (self->x > enemy->target_x)
    {
        physics_vec2_push(&self->dx, &self->dy, 180, move_speed);
    } 

    if ((((int)application_get_time()) % 3) == 0 && get_random_int(0, 20) == 0)
    {
        actor_bullet_T* bullet = init_actor_bullet(self->x, self->y, self->z, _bullet_check_collision);

        bullet->r = 255;
        bullet->g = 0;
        bullet->b = 0;

        (((actor_T*)bullet)->sprite)->flip_y = 1;
        
        actor_bullet_push((actor_T*)bullet, 270, 700.0f); 

        scene_add_actor(scene, (actor_T*) bullet);
    }

    move(self, self->dx * APP->delta_time, self->dy * APP->delta_time);
}
