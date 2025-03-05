#include <json.hpp>
#include <salamander/json_api.h>
#include <string>
#include <cstring>
#include <fstream>
#include <ostream>

struct Json_t
{
    nlohmann::json json;
};

struct JsonIterator_t
{
    nlohmann::json::iterator it;
};

extern "C"
{

Json Json_Create()
{
    Json j = new Json_t();
    return j;
}

void Json_Destroy(Json j)
{
    delete j;
}

void Json_CreateArray(Json j)
{
    j->json = nlohmann::json::array();
}

void Json_SaveBool(Json j, const char* name, const bool val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void Json_SaveString(Json j, const char* name, const char* val)
{
    if (j && val)
    {
        j->json[name] = std::string(val);
    }
}

void Json_SaveInt(Json j, const char* name, const int val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void Json_SaveFloat(Json j, const char* name, const float val)
{
    if (j)
    {
        j->json[name] = val;
    }
}

void Json_SaveDouble(Json j, const char* name, const double val)
{
    if (j)
    {
        j->json = val;
        j->json[name] = val;
    }
}

void Json_SaveVec2(Json j, const char* name, const vec2 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1]};
    }
}

void Json_SaveVec3(Json j, const char* name, const vec3 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1], val[2]};
    }
}

void Json_SaveVec4(Json j, const char* name, const vec4 val)
{
    if (j)
    {
        j->json[name] = {val[0], val[1], val[2], val[3]};
    }
}

void Json_SaveMat4(Json j, const char* name, const mat4 val)
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

void Json_LoadBool(Json j, const char* key, bool* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<bool>();
    }
}

void Json_LoadString(Json j, const char* key, char* val)
{
    if (j && key && val && j->json.contains(key))
    {
        std::string str = j->json[key].get<std::string>();
        strcpy(val, str.c_str());
    }
}

void Json_LoadInt(Json j, const char* key, int* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<int>();
    }
}

void Json_LoadFloat(Json j, const char* key, float* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<float>();
    }
}

void Json_LoadDouble(Json j, const char* key, double* val)
{
    if (j && key && val && j->json.contains(key))
    {
        *val = j->json[key].get<double>();
    }
}

void Json_LoadVec2(Json j, const char* key, vec2 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
    }
}

void Json_LoadVec3(Json j, const char* key, vec3 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
        val[2] = j->json[key][2];
    }
}

void Json_LoadVec4(Json j, const char* key, vec4 val)
{
    if (j && key && val && j->json.contains(key))
    {
        val[0] = j->json[key][0];
        val[1] = j->json[key][1];
        val[2] = j->json[key][2];
        val[3] = j->json[key][3];
    }
}

void Json_LoadMat4(Json j, const char* key, mat4 val)
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

void Json_PushBack(Json j, const Json val)
{
    if (j)
    {
        j->json.push_back(val->json);
    }
}

void Json_Iterate(Json j, JsonIteratorFunc sys)
{
    for (nlohmann::json& childJ : j->json)
    {
        Json childJJ = {};
        childJJ->json = childJ;
        sys(childJJ);
    }
}

Json Json_GetJsonAtIndex(Json j, int index)
{
    Json json = Json_Create();
    json->json = j->json[index];
    return json;
}

int Json_GetJsonArraySize(Json j)
{
    return j->json.size();
}

bool Json_HasKey(Json j, const char* key)
{
    return j->json.contains(key);
}

bool Json_SaveToFile(Json j, const char* filename)
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

Json Json_LoadFromFile(const char* filename)
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

        // Create a new Json object
        Json j = Json_Create();

        // Parse the file contents
        nlohmann::json parsedJson = nlohmann::json::parse(file);

        // Set the parsed JSON to our Json object
        j->json = parsedJson;

        return j;
    }
    catch (...)
    {
        // In case of any parsing or file reading error
        return nullptr;
    }
}
}

nlohmann::json Json_GetJson(Json j)
{
    return j->json;
}

void Json_SetJson(Json j, const nlohmann::json& json)
{
    j->json = json;
}
