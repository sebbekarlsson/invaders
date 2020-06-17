#include "include/main_scene.h"
#include "include/actor_player.h"
#include "include/actor_enemy.h"
#include <cengine/window.h>
#include <cengine/random.h>
#include <cengine/draw.h>
#include <cengine/application.h>
#include <cengine/keyboard.h>
#include <sys/param.h>
#include <string.h>


extern application_T* APP;
extern texture_T* TEXTURE_BG;

main_scene_T* init_main_scene()
{
    main_scene_T* main_scene = calloc(1, sizeof(struct MAIN_SCENE_STRUCT));
    scene_T* scene = scene_constructor((scene_T*) main_scene, 2);

    scene->tick = main_scene_tick;
    scene->draw = main_scene_draw;
    scene->pre_draw = main_scene_predraw;
    
    main_scene->enemies = init_dynamic_list(sizeof(struct ACTOR_STRUCT*));
    main_scene->bg_shift = 0;

    main_scene_setup(main_scene);

    return main_scene;
}

void main_scene_setup(main_scene_T* main_scene)
{
    scene_T* scene = (scene_T*) main_scene;

    main_scene->player = (actor_player_T*) scene_add_actor(
        scene,
        (actor_T*)
        init_actor_player((RES_WIDTH / 2) - (32 / 2), RES_HEIGHT - 32, 0.0f) 
    );

    main_scene->difficulty = 0;
}

static void _actor_free(void* x)
{
    actor_T* actor = (actor_T*) x;
    actor_free(actor);
}

void main_scene_clear(main_scene_T* main_scene)
{
    scene_T* scene = (scene_T*) main_scene;

    size_t xyz = scene->actors->size; 
    for (int i = 0; i < xyz; i++)
        actor_free((actor_T*)scene->actors->items[i]);

    free(main_scene->enemies->items);
    main_scene->enemies->items = (void*)0;
    main_scene->enemies->size = 0;
    free(scene->actors->items);
    scene->actors->items = (void*)0;
    scene->actors->size = 0;

    main_scene->difficulty = 0;
}

typedef float light[3];

void main_scene_predraw(scene_T* self)
{
    main_scene_T* main_scene = (main_scene_T*) self;

    /*glUniform1i(glGetUniformLocation(APP->shader_program_default, "lighting_enabled"), 1);
    glUniform1f(glGetUniformLocation(APP->shader_program_default, "lighting_max_darkness"), 1.80f);

    light* lights = calloc(256, sizeof(sizeof(float) * 3));
    for (int i = 0; i < main_scene->enemies->size; i++)
    {
        actor_T* a = (actor_T*) main_scene->enemies->items[i];
        lights[i][0] = a->x;
        lights[i][1] = a->y;
        lights[i][2] = a->z;
    }

    glUniform3fv(glGetUniformLocation(APP->shader_program_default, "light_pos"), main_scene->enemies->size, *lights);
    glUniform1i(glGetUniformLocation(APP->shader_program_default, "nr_lights"), main_scene->enemies->size);

    free(lights);

    glUniform1i(glGetUniformLocation(APP->shader_program_default, "lighting_enabled"), 0);*/
    draw_options_T options = DRAW_OPTIONS_INIT;
    options.x = 0;
    options.y = 0;
    options.z = -1;
    options.pan_y = -main_scene->bg_shift;
    options.width = TEXTURE_BG->width;
    options.height = TEXTURE_BG->height;
    options.texture = TEXTURE_BG->id;
    options.r = 255;
    options.g = 255;
    options.b = 255;

    draw_texture(
        self->draw_program,
        options        
    );

    if (!self->paused)
        main_scene->bg_shift += 0.4f * APP->delta_time;
    //glUniform1i(glGetUniformLocation(APP->shader_program_default, "lighting_enabled"), 1);
}

void main_scene_draw(scene_T* self)
{
    main_scene_T* main_scene = (main_scene_T*) self;

    draw_options_T options = DRAW_OPTIONS_INIT;
    options.text = LOSE_TEXT;
    options.fontpath = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";
    int font_size = 77;
    options.x = (RES_WIDTH / 2) - ((strlen(LOSE_TEXT) * (font_size / 2) + font_size) / 2);
    options.y = RES_HEIGHT / 2;
    options.z = 1;
    options.font_size = font_size;
    options.r = 255;
    options.g = 255;
    options.b = 255;
    
    if (((actor_ship_T*)main_scene->player)->health <= 0)
    {
        draw_text(
            self->draw_program_text,
            options
        );
    }

    draw_options_T options2 = DRAW_OPTIONS_INIT;
    options2.fontpath = options.fontpath;
    int font_size2 = 12;
    options2.x = 0;
    options2.y = font_size2;
    options2.z = 1;
    options2.font_size = font_size2;
    options2.r = 255;
    options2.g = 255;
    options2.b = 255;

    const char* template = "Level: %d";
    char* dif_str = calloc(strlen(template) + 128, sizeof(char));
    sprintf(dif_str, template, main_scene->difficulty);
    
    options2.text = dif_str;

    draw_text(
        self->draw_program_text,
        options2
    );

    free(dif_str);
}

void main_scene_tick(scene_T* self)
{
    main_scene_T* main_scene = (main_scene_T*) self;

    if (
        main_scene->enemies->size == 0 &&
        get_random_int(0, 100) == 0
    )
    {
        int min_enemies = 1;
        int max_enemies = MAX(1, main_scene->difficulty);
        int nr_enemies = get_random_int(min_enemies, max_enemies);

        for (int i = 0; i < nr_enemies; i++)
            main_scene_spawn_enemy(main_scene);
    }

    if (keyboard_check_pressed_once(GLFW_KEY_P))
        self->paused = !self->paused;
}

void main_scene_spawn_enemy(main_scene_T* main_scene)
{
    float x = (float) get_random_int(0, (RES_WIDTH - 32) / 32) * 32;
    float y = (float) get_random_int(0, MIN(RES_HEIGHT, RES_HEIGHT * (MAX(1, main_scene->difficulty)*0.5)) / 32) * 32;
    
    printf("Spawning an enemy at: %12.6f, %12.6f\n", x, y);

    actor_T* actor = scene_add_actor(
        (scene_T*) main_scene,
        (actor_T*)
        init_actor_enemy(MAX(0, x), -y, 0.0f) 
    );

    dynamic_list_append(main_scene->enemies, actor);
}

void main_scene_advance(main_scene_T* main_scene)
{
    main_scene->difficulty += 1;
}
