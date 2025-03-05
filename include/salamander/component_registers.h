#include <salamander/components.h>
#include <salamander/json_api.h>

void smName_Draw(smName* name);
Json smName_Save(smName* name);
void smName_Load(smName* name, Json j);

void smSpriteRenderer_Draw(smSpriteRenderer* sprite);
Json smSpriteRenderer_Save(smSpriteRenderer* sprite);
void smSpriteRenderer_Load(smSpriteRenderer* sprite, Json j);
