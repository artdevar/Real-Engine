#pragma once

#include <string>

class IIdentifiable
{
public:

  virtual ~IIdentifiable() = default;

  const std::string & GetID() const
  {
    return m_ID;
  }

  void SetID(const std::string & _ID)
  {
    m_ID = _ID;
  }

protected:

  std::string m_ID;
};