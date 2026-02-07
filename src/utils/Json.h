#pragma once

#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>

namespace utils
{
  inline bool IsJson(const std::filesystem::path &_Path)
  {
    return _Path.extension() == ".json";
  }

  inline nlohmann::json ParseJson(const std::filesystem::path &_Path)
  {
    nlohmann::json Rv;

    std::ifstream File(_Path);
    if (File.is_open())
      File >> Rv;

    return Rv;
  }

  inline void SaveJson(const std::filesystem::path &_Path, const nlohmann::json &_Data)
  {
    std::ofstream ConfigFile(_Path);
    assert(ConfigFile.is_open());
    ConfigFile << _Data;
  }

  template <typename T>
  inline nlohmann::json ToJson(T _Val)
  {
    uint8_t Data[sizeof(T)] = {0};
    memcpy(Data, glm::value_ptr(_Val), sizeof(T));

    return nlohmann::json(Data);
  }

  template <typename T>
  inline T FromJson(const nlohmann::json &_Json)
  {
    uint8_t Data[sizeof(T)] = {0};
    _Json.get_to<uint8_t, sizeof(T)>(Data);

    T Val;
    memcpy(glm::value_ptr(Val), Data, sizeof(T));
    return Val;
  }

  template <typename T>
  inline void FromJson(const nlohmann::json &_Json, T &_Dst)
  {
    uint8_t Data[sizeof(T)] = {0};
    _Json.get_to<uint8_t, sizeof(T)>(Data);

    memcpy(glm::value_ptr(_Dst), Data, sizeof(T));
  }

}