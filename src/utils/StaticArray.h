#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <ranges>
#include <algorithm>
#include "Common.h"

template <typename T, std::size_t Capacity>
class CStaticArray final
{
  DISABLE_CLASS_COPY(CStaticArray);

public:
  class CIterator;

  constexpr CStaticArray()
  {
    if constexpr (std::is_trivially_default_constructible_v<T>)
      std::memset(m_Data, 0x00, sizeof(m_Data));
  }

  constexpr CStaticArray(std::initializer_list<T> _InitList)
  {
    Assign(_InitList);
  }

  template <std::convertible_to<T> U>
  constexpr CStaticArray(U &&_Value)
  {
    Fill(std::forward<U>(_Value));
  }

  constexpr CStaticArray &operator=(std::initializer_list<T> _InitList)
  {
    Assign(_InitList);
    return *this;
  }

  constexpr T &operator[](std::size_t _Index)
  {
    return m_Data[_Index];
  }

  constexpr const T &operator[](std::size_t _Index) const
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

  T *Data()
  {
    return m_Data;
  }

  const T *Data() const
  {
    return m_Data;
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

  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>
  constexpr void Assign(R &&_Range)
  {
    assert(_Range.size() <= GetCapacity() && "Initializer list too large");

    const size_t AmountToCopy = std::min(static_cast<std::size_t>(_Range.size()), GetCapacity());
    std::copy(std::ranges::begin(_Range), std::ranges::begin(_Range) + AmountToCopy, m_Data);
    m_ActualSize = AmountToCopy;
  }

  template <std::convertible_to<T> U>
  constexpr void Fill(U &&_Value)
  {
    m_ActualSize = GetCapacity();
    std::fill(begin(), end(), static_cast<T>(std::forward<U>(_Value)));
  }

  constexpr void Clear()
  {
    m_ActualSize = 0;
  }

  template <std::convertible_to<T> U>
  constexpr void PushBack(U &&_Value)
  {
    assert(GetActualSize() < GetCapacity() && "Array is filled up");

    m_Data[m_ActualSize] = static_cast<T>(std::forward<U>(_Value));
    m_ActualSize++;
  }

  constexpr T &Back()
  {
    assert(!IsEmpty());

    return m_Data[m_ActualSize - 1];
  }

  constexpr const T &Back() const
  {
    assert(!IsEmpty());

    return m_Data[m_ActualSize - 1];
  }

  constexpr void PopBack()
  {
    assert(!IsEmpty());

    if constexpr (std::is_trivially_default_constructible_v<T>)
      std::memset(&m_Data[m_ActualSize - 1], 0x00, sizeof(T));

    m_ActualSize--;
  }

private:
  T m_Data[Capacity];
  std::size_t m_ActualSize = 0;
};

template <typename T, std::size_t N>
class CStaticArray<T, N>::CIterator final
{
public:
  explicit constexpr CIterator(T *_Item) : m_Item(_Item) {}

  constexpr T &operator*()
  {
    return *m_Item;
  }

  constexpr T *operator->()
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

  constexpr bool operator==(const CIterator &_Other) const
  {
    return m_Item == _Other.m_Item;
  }

  constexpr bool operator!=(const CIterator &_Other) const
  {
    return m_Item != _Other.m_Item;
  }

private:
  T *m_Item;
};