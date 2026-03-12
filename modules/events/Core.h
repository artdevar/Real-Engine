#pragma once

#include <common/MathTypes.h>
#include <variant>

enum class TEventType; // Declared on lower level

struct TEvent
{
  using TEventData = std::variant<std::monostate, //
                                  bool,           //
                                  char,           //
                                  int,            //
                                  unsigned int,   //
                                  float,          //
                                  double,         //
                                  TVector2i,      //
                                  TVector2f,      //
                                  TVector3i,      //
                                  TVector3f,      //
                                  TVector4i,      //
                                  TVector4f,      //
                                  TColor,         //
                                  TRecti,         //
                                  TRectf>;

  TEventData Value;
  TEventType Type;

  template <typename T>
  const T &GetValue() const
  {
    return std::get<T>(Value);
  }
};