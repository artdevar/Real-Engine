#pragma once

#include <nlohmann/json_fwd.hpp>

class ISerializable
{
public:
  virtual ~ISerializable() = default;

  virtual void ReadFromData(const nlohmann::json &_Data) = 0;

  virtual nlohmann::json GetSaveData() const = 0;
};