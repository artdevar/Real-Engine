#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "Common.h"

template <typename T, std::size_t Capacity>
class CStaticArray final
{
  DISABLE_CLASS_COPY(CStaticArray);

public:
  class CIterator;

  constexpr CStaticArray() = default;

  template <std::convertible_to<T> U>
  constexpr CStaticArray(U && _Value)
  {
    FillArray(std::forward<U>(_Value));
  }

  constexpr T & operator[](std::size_t _Index)
  {
    return m_Data[_Index];
  }

  constexpr const T & operator[](std::size_t _Index) const
  {
    return m_Data[_Index];
  }

  constexpr std::size_t GetCapacity() const
  {
    return Capacity;
  }

  constexpr std::size_t GetActualSize() const
  {
    return m_ActualSize;
  }

  constexpr bool IsEmpty() const
  {
    return m_ActualSize == 0;
  }

  constexpr CIterator begin()
  {
    return CIterator(m_Data);
  }

  constexpr CIterator begin() const
  {
    return CIterator(m_Data);
  }

  constexpr CIterator end()
  {
    return CIterator(m_Data + GetActualSize());
  }

  constexpr CIterator end() const
  {
    return CIterator(m_Data + GetActualSize());
  }

  template <std::convertible_to<T> U>
  constexpr void FillArray(U && _Value)
  {
    m_ActualSize = GetCapacity();
    std::fill(begin(), end(), std::forward<U>(_Value));
  }

  constexpr void Clear()
  {
    m_ActualSize = 0;
  }

  template <std::convertible_to<T> U>
  constexpr void PushBack(U && _Value)
  {
    assert(GetActualSize() < GetCapacity() && "Array is filled up");

    m_Data[m_ActualSize] = std::forward<U>(_Value);
    m_ActualSize++;
  }

  constexpr void PopBack()
  {
    assert(!IsEmpty());

    m_ActualSize--;
  }

private:

  T           m_Data[Capacity];
  std::size_t m_ActualSize = 0;
};

template <typename T, std::size_t N>
class CStaticArray<T, N>::CIterator final
{
public:

  explicit constexpr CIterator(T * _Item) : m_Item(_Item) {}

  constexpr T & operator*()
  {
    return *m_Item;
  }

  constexpr T * operator->()
  {
    return m_Item;
  }

  constexpr CIterator &operator++()
  {
    ++m_Item;
    return *this;
  }

  constexpr CIterator operator++(int)
  {
    CIterator Temp = *this;
    ++(*this);
    return Temp;
  }

  constexpr bool operator==(const CIterator & _Other) const
  {
    return m_Item == _Other.m_Item;
  }

  constexpr bool operator!=(const CIterator & _Other) const
  {
    return m_Item != _Other.m_Item;
  }

private:

  T * m_Item;
};