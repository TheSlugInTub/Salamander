#pragma once

#include <salamander/components.h>
#include <salamander/json_api.h>

void   smName_Draw(smName* name);
smJson smName_Save(smName* name);
void   smName_Load(smName* name, smJson j);

void   smTransform_Draw(smTransform* trans);
smJson smTransform_Save(smTransform* trans);
void   smTransform_Load(smTransform* trans, smJson j);

void   smSpriteRenderer_Draw(smSpriteRenderer* sprite);
smJson smSpriteRenderer_Save(smSpriteRenderer* sprite);
void   smSpriteRenderer_Load(smSpriteRenderer* sprite, smJson j);
