#include "salamander/json_api.h"
#include "salamander/window.h"
#include <salamander/salamander.h>

typedef struct
{
    int x, y;
} Bruh;

void Bruh_Draw(Bruh* bruh)
{
    printf("BruhDraw");
}

Json Bruh_Save(Bruh* bruh)
{
    printf("BruhSave");
    Json j = Json_Create(j);
    return j;
}

void Bruh_Load(Bruh* bruh, Json j)
{
    printf("BruhLoad");
}

int main(int argc, char** argv)
{
    smWindow window;
    smWindow_Create(&window, "Bombratter", 1920, 1080, false, true);

    SceneHandle scene = ECS_CreateScene();

    smEngineState state;
    state.scene = scene;
    state.window = &window;

    smSetState(&state);

    EntityID ent = ECS_AddEntity(scene);
    EntityID ent2 = ECS_AddEntity(scene);

    Bruh* trans = ECS_ASSIGN(scene, ent, Bruh);
    trans->x = 69;
    trans->y = 420;
    Bruh* trans2 = ECS_ASSIGN(scene, ent2, Bruh);
    trans2->x = 14;
    trans2->y = 145;

    REGISTER_COMPONENT(Bruh, Bruh_Draw, Bruh_Save, Bruh_Load);
        
    Json j = Json_Create();
    Registry_SaveComponents(j);

    while (!smWindow_ShouldClose(&window))
    {

        smWindow_Update(&window);
    }

    // ECS_ITER_START(scene, ECS_COMPONENT_TYPE(Transform))
    // {
    //     Transform* pos = ECS_GET(scene, _entity, Transform);
    //     printf("Pos x: %d", pos->x);
    // }
    // ECS_ITER_END();

    system("pause");

    return 0;
}
