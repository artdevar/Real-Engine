#pragma once

#include <common/Common.h>
#include <memory>

template <typename T>
class CSharable : public std::enable_shared_from_this<T>
{
  DISABLE_CLASS_COPY(CSharable);

protected:
  CSharable()  = default;
  ~CSharable() = default;

public:
  using TSharedPtr = std::shared_ptr<T>;
  using TWeakPtr   = std::weak_ptr<T>;

  template <typename... Args>
  static TSharedPtr Create(Args &&..._Args)
  {
    return std::make_shared<T>(std::forward<Args>(_Args)...);
  }

  TSharedPtr GetSharedPtr()
  {
    assert(IsShared() && "Object is not managed by a shared pointer");
    return std::static_pointer_cast<T>(this->shared_from_this());
  }

  TWeakPtr GetWeakPtr()
  {
    assert(IsShared() && "Object is not managed by a shared pointer");
    return this->weak_from_this();
  }

private:
  bool IsShared() const
  {
    return this->weak_from_this().use_count() > 0;
  }
};