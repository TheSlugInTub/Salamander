#pragma once

#include <salamander/vector.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <salamander/json_api.h>
#include <salamander/shader.h>
#include <salamander/components.h>

#define MAX_BONE_INFLUENCE 4

typedef struct
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;

    int   boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
} smVertex;

typedef struct
{
    unsigned int ID;
    char         type[64];
    char         path[64];
} smTexture;

typedef struct
{
    smVector*    vertices; // vec2
    smVector*    indices;  // unsigned int
    smVector*    textures; // smTexture
    unsigned int VAO, VBO, EBO;
} smMesh;

smMesh smMesh_Create(smVector* meshVertices, smVector* meshIndices,
                     smVector* meshTextures);
void   smMesh_SetupBones(smMesh* mesh);

typedef struct
{
    int  ID;
    mat4 offset;
} smBoneInfo;

// Map replacement using a simple array (could be replaced with a
// proper hash map)
typedef struct
{
    char       name[128];
    smBoneInfo info;
    bool       used;
} smBoneMapEntry;

typedef struct
{
    smBoneMapEntry* entries;
    int             capacity;
    int             count;
} smBoneInfoMap;

void        smBoneInfoMap_Create(smBoneInfoMap* map, int capacity);
void        smBoneInfoMap_Destroy(smBoneInfoMap* map);
smBoneInfo* smBoneInfoMap_Find(smBoneInfoMap* map, const char* name);
void        smBoneInfoMap_Insert(smBoneInfoMap* map, const char* name,
                                 smBoneInfo info);

typedef struct
{
    char      modelPath[128];
    char      directory[128];
    smVector* loadedTextures; // smTexture
    smVector* meshes;         // smMesh
    bool      gammaCorrection;
    bool      extractTexture;

    unsigned int texture;
    char         texturePath[128];

    unsigned int normalTexture;
    char         normalTexturePath[128];

    unsigned int specularTexture;
    char         specularTexturePath[128];

    smBoneInfoMap boneInfoMap;
    int           boneCounter;
} smModel;

void   smModel_Create(smModel* model);
void   smModel_Load(smModel* model, const char* path);
void   smModel_ProcessNode(smModel* model, struct aiNode* node,
                           const struct aiScene* scene);
smMesh smModel_ProcessMesh(smModel* model, struct aiMesh* mesh,
                           const struct aiScene* scene);
void smModel_Draw(smModel* mesh, smTransform* trans, smShader shader);
void smModel_SetVertexBoneDataToDefault(smVertex* vertex);
void smModel_SetVertexBoneData(smVertex* vertex, int boneID,
                               float weight);
void smModel_ExtractBoneWeightForVertices(
    smModel* model, smVector* vertices, struct aiMesh* mesh,
    const struct aiScene* scene);
void smModel_LoadMaterialTextures(smModel*              model,
                                  struct aiMaterial*    mat,
                                  enum aiTextureType    type,
                                  const char*           typeName,
                                  const struct aiScene* scene,
                                  smVector*             textures);

unsigned int smTextureFromFile(const char* path,
                               const char* directory);

unsigned int
smTextureFromEmbeddedData(const struct aiTexture* texture);

typedef smModel smMeshRenderer;

void smMeshRenderer_StartSys();
void smMeshRenderer_Sys();

void   smMeshRenderer_Draw(smMeshRenderer* mesh);
smJson smMeshRenderer_Save(smMeshRenderer* mesh);
void   smMeshRenderer_Load(smMeshRenderer* mesh, smJson j);

void smAssimpVec3ToGLM(const struct aiVector3D* from, vec3 to);
void smAssimpMat4ToGLM(const struct aiMatrix4x4* from, mat4 to);
