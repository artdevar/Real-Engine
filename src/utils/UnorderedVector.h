#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
class CUnorderedVector
{
public:
  using IteratorType      = std::vector<T>::iterator;
  using ConstIteratorType = std::vector<T>::const_iterator;

  constexpr IteratorType begin()
  {
    return m_Data.begin();
  }

  constexpr IteratorType end()
  {
    return m_Data.end();
  }

  constexpr ConstIteratorType begin() const
  {
    return m_Data.begin();
  }

  constexpr ConstIteratorType end() const
  {
    return m_Data.end();
  }

  constexpr T &operator[](size_t _Index)
  {
    return m_Data[_Index];
  }

  constexpr const T &operator[](size_t _Index) const
  {
    return m_Data[_Index];
  }

  constexpr bool Empty() const
  {
    return m_Data.empty();
  }

  constexpr size_t Size() const
  {
    return m_Data.size();
  }

  constexpr void Reserve(size_t _Elements)
  {
    m_Data.reserve(_Elements);
  }

  constexpr void Erase(IteratorType _Iterator)
  {
    *_Iterator = std::move(m_Data.data()[Size() - 1]);
    m_Data.pop_back();
  }

  constexpr void Erase(const T &_Value)
  {
    const IteratorType Iterator = Find(_Value);
    assert(Iterator != end());
    Erase(Iterator);
  }

  constexpr bool SafeErase(const T &_Value)
  {
    const IteratorType Iterator = Find(_Value);
    const bool         IsFound  = Iterator != end();

    if (IsFound)
      Erase(Iterator);

    return IsFound;
  }

  constexpr IteratorType Find(const T &_Value)
  {
    return std::find(begin(), end(), _Value);
  }

  constexpr ConstIteratorType Find(const T &_Value) const
  {
    return std::find(begin(), end(), _Value);
  }

  constexpr bool Contains(const T &_Value) const
  {
    return Find(_Value) != end();
  }

  template <std::convertible_to<T> U>
  constexpr void Push(U &&_Value)
  {
    m_Data.push_back(std::forward<U>(_Value));
  }

  template <std::convertible_to<T> U>
  constexpr bool PushUnique(U &&_Value)
  {
    const bool NeedPush = !Contains(std::forward<U>(_Value));

    if (NeedPush)
      Push(std::forward<U>(_Value));

    return NeedPush;
  }

  friend std::ostream &operator<<(std::ostream &_Stream, const CUnorderedVector &_Vector)
  {
    _Stream << "{ ";
    for (const T &Val : _Vector.m_Data)
      _Stream << Val << ' ';
    _Stream << "}\n";

    return _Stream;
  }

private:
  std::vector<T> m_Data;
};