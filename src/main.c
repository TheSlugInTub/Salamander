#include <salamander/salamander.h>

int main(int argc, char** argv)
{
    smWindow window = smWindow_Create("Bombratter", 1920,
                                      1080, false, true);

    SceneHandle scene = ECS_CreateScene();

    smEngineState state;
    state.scene = scene;
    state.window = &window;

    smSetState(&state);

    while (!smWindow_ShouldClose(&window))
    {

        smWindow_Update(&window);
    }

    return 0;
}
