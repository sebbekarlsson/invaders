#include "include/actor_ship.h"
#include "include/main_scene.h"
#include <sys/param.h>
#include <cengine/application.h>


extern application_T* APP;
extern unsigned int TEXTURE_HEART_ID;

actor_ship_T* actor_ship_constructor(actor_ship_T* actor, float x, float y, float z, float health)
{
    actor_T* base_actor = actor_constructor((actor_T*) actor, x, y, z);
    actor->health = health;

    base_actor->draw = actor_ship_draw;

    actor->died_at = 0;
    actor->protection = 0;

    return actor;
}

void actor_ship_tick(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);
    main_scene_T* main_scene = (main_scene_T*) scene;
    
    actor_ship_T* ship = (actor_ship_T*) self;

    if (ship->health <= 0 && ship->on_death && ship->died_at == 0)
    {
        ship->died_at = application_get_time();
        ship->on_death(self);
    }
}

void actor_ship_draw(actor_T* self)
{
    scene_T* scene = application_get_current_scene(APP);

    actor_ship_T* ship = (actor_ship_T*) self;
    unsigned int flip = self->sprite->flip_y;
    
    int size = 8;
    int nr_hearts = (int)(ship->health / 10);
    int full_width = (nr_hearts) * size;
    for (int i = 0; i < nr_hearts; i++)
    {
        draw_options_T options = DRAW_OPTIONS_INIT;
        options.x = ((self->x + 16) + (i * size)) - (full_width / 2);
        options.y = flip ? self->y + 32 : self->y - size;
        options.z = self->z;
        options.width = size;
        options.height = size;
        options.texture = TEXTURE_HEART_ID;
        options.r = 255;
        options.g = 255;
        options.b = 255;
        options.a = 0.3f;

        draw_texture(
            scene->draw_program,
            options        
        );
    }
}

void actor_ship_damage(actor_ship_T* ship, float damage)
{
    ship->health = MAX(0, ship->health - damage);
}
