#include <json.hpp>
#include <salamander/json_api.h>
#include <string>
#include <cstring>
#include <fstream>

struct smJson_t
{
    nlohmann::json json;
};

struct smJsonIterator_t
{
    nlohmann::json::iterator it;
};

extern "C"
{

smJson smJson_Create()
{
    smJson j = new smJson_t();
    return j;
}

void smJson_Destroy(smJson j)
{
    delete j;
}

void smJson_CreateArray(smJson j)
{
    j->json = nlohmann::json::array();
}

void smJson_SaveBool(smJson j, const char* name, const bool val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void smJson_SaveString(smJson j, const char* name, const char* val)
{
    if (j && val)
    {
        j->json[name] = std::string(val);
    }
}

void smJson_SaveInt(smJson j, const char* name, const int val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void smJson_SaveFloat(smJson j, const char* name, const float val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void smJson_SaveDouble(smJson j, const char* name, const double val)
{
    if (j)
    {
        j->json = val;
        j->json[name] = val;
    }
}

void smJson_SaveVec2(smJson j, const char* name, const vec2 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1]};
    }
}

void smJson_SaveVec3(smJson j, const char* name, const vec3 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1], val[2]};
    }
}

void smJson_SaveVec4(smJson j, const char* name, const vec4 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1], val[2], val[3]};
    }
}

void smJson_SaveMat4(smJson j, const char* name, const mat4 val)
{
    if (j)
    {
        nlohmann::json matrix;
        j->json[name] = {val[0],  val[1],  val[2],  val[3],
                         val[4],  val[5],  val[6],  val[7],
                         val[8],  val[9],  val[10], val[11],
                         val[12], val[13], val[14], val[15]};
    }
}

void smJson_LoadBool(smJson j, const char* key, bool* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<bool>();
    }
}

void smJson_LoadString(smJson j, const char* key, char* val)
{
    if (j && key && val && j->json.contains(key))
    {
        std::string str = j->json[key].get<std::string>();
        strcpy(val, str.c_str());
    }
}

void smJson_LoadInt(smJson j, const char* key, int* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<int>();
    }
}

void smJson_LoadFloat(smJson j, const char* key, float* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<float>();
    }
}

void smJson_LoadDouble(smJson j, const char* key, double* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<double>();
    }
}

void smJson_LoadVec2(smJson j, const char* key, vec2 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
    }
}

void smJson_LoadVec3(smJson j, const char* key, vec3 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
        val[2] = j->json[key][2];
    }
}

void smJson_LoadVec4(smJson j, const char* key, vec4 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
        val[2] = j->json[key][2];
        val[3] = j->json[key][3];
    }
}

void smJson_LoadMat4(smJson j, const char* key, mat4 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0][0] = j->json[key][0];
        val[0][1] = j->json[key][1];
        val[0][2] = j->json[key][2];
        val[0][3] = j->json[key][3];
        val[1][0] = j->json[key][5];
        val[1][1] = j->json[key][4];
        val[1][2] = j->json[key][5];
        val[1][3] = j->json[key][6];
        val[2][0] = j->json[key][7];
        val[2][1] = j->json[key][8];
        val[2][2] = j->json[key][9];
        val[2][3] = j->json[key][10];
        val[3][0] = j->json[key][11];
        val[3][1] = j->json[key][12];
        val[3][2] = j->json[key][13];
        val[3][3] = j->json[key][14];
    }
}

void smJson_PushBack(smJson j, const smJson val)
{
    if (j)
    {
        j->json.push_back(val->json);
    }
}

void smJson_Iterate(smJson j, smJsonIteratorFunc sys)
{
    for (nlohmann::json& childJ : j->json)
    {
        smJson childJJ = {};
        childJJ->json = childJ;
        sys(childJJ);
    }
}

smJson smJson_GetsmJsonAtIndex(smJson j, int index)
{
    smJson json = smJson_Create();
    json->json = j->json[index];
    return json;
}

int smJson_GetsmJsonArraySize(smJson j)
{
    return j->json.size();
}

bool smJson_HasKey(smJson j, const char* key)
{
    return j->json.contains(key);
}

bool smJson_SaveToFile(smJson j, const char* filename)
{
    if (j == nullptr || filename == nullptr)
    {
        return false;
    }

    try
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        file << j->json.dump(4); // dump with 4-space indentation
        file.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

smJson smJson_LoadFromFile(const char* filename)
{
    if (filename == nullptr)
    {
        return nullptr;
    }

    try
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return nullptr;
        }

        // Create a new smJson object
        smJson j = smJson_Create();

        // Parse the file contents
        nlohmann::json parsedsmJson = nlohmann::json::parse(file);

        // Set the parsed JSON to our smJson object
        j->json = parsedsmJson;

        return j;
    }
    catch (...)
    {
        // In case of any parsing or file reading error
        return nullptr;
    }
}
}

nlohmann::json smJson_GetsmJson(smJson j)
{
    return j->json;
}

void smJson_SetsmJson(smJson j, const nlohmann::json& json)
{
    j->json = json;
}
