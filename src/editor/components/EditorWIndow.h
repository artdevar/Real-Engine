#pragma once

#include <common/MathTypes.h>
#include <string>

class IEditorWindow
{
public:
  virtual ~IEditorWindow() = default;

  virtual std::string GetName() const = 0;
  virtual TVector2i GetSize() const   = 0;
};