#include <cengine/main.h>
#include <cengine/application.h>
#include <cengine/texture.h>
#include "include/main_scene.h"


extern application_T* APP;

texture_T* TEXTURE_SHEET;
texture_T* TEXTURE_BG;
unsigned int TEXTURE_HEART_ID;

int main(int argc, char* argv[])
{
    APP = init_application(640, 480, "App");

    TEXTURE_SHEET = texture_get("res/sheet.png");
    TEXTURE_BG = texture_get("res/space.png");
    TEXTURE_HEART_ID = texture_get_cut_id(TEXTURE_SHEET, 32 * 3, 0, 32, 32);

    application_add_scene(APP, (scene_T*) init_main_scene());

    return cengine_main(argc, argv);
}
