#pragma once

#include <memory>

template <typename T>
class CSharable : public std::enable_shared_from_this<T>
{
public:
  using SharedPtr = std::shared_ptr<T>;
  using WeakPtr   = std::weak_ptr<T>;

  template <typename... Args>
  static SharedPtr Create(Args &&..._Args)
  {
    return std::make_shared<T>(std::forward<Args>(_Args)...);
  }

  SharedPtr GetSharedPtr()
  {
    assert(IsShared() && "Object is not managed by a shared pointer");
    return std::static_pointer_cast<T>(this->shared_from_this());
  }

  WeakPtr GetWeakPtr()
  {
    assert(IsShared() && "Object is not managed by a shared pointer");
    return this->weak_from_this();
  }

private:
  bool IsShared() const
  {
    return !this->weak_from_this().expired();
  }
};