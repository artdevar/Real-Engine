#pragma once

#include <unordered_map>
#include <optional>

template <typename Key, typename Value>
class CLRUCache
{
  struct TNode
  {
    Key    first;
    Value  second;
    TNode *next;
    TNode *prev;
  };

public:
  CLRUCache(std::optional<size_t> _Capacity) :
      m_Capacity(std::move(_Capacity))
  {
    if (m_Capacity)
      m_Cache.reserve(*m_Capacity);
  }

  const Value &Get(const Key &key)
  {
    const auto Iter = m_Cache.find(key);
    Refresh(&Iter->second);
    return Iter->second.second;
  }

  void Put(const Key &_Key, Value _Value)
  {
    TNode Node{
        .first  = _Key,
        .second = std::move(_Value),
        .next   = nullptr,
        .prev   = nullptr,
    };

    const auto [Iter, IsInserted] = m_Cache.insert_or_assign(_Key, Node);
    if (IsInserted)
    {
      MarkMostRecent(&Iter->second);
      if (m_Capacity && m_Cache.size() > *m_Capacity)
        m_Cache.erase(PopLeastRecent());
    }
    else
    {
      Refresh(&Iter->second);
    }
  }

private:
  void Refresh(TNode *_Node)
  {
    RemoveNode(_Node);
    MarkMostRecent(_Node);
  }

  void MarkMostRecent(TNode *_Node)
  {
    if (!m_Head)
    {
      m_Head = m_Tail = _Node;
      return;
    }

    _Node->next  = m_Head;
    m_Head->prev = _Node;
    m_Head       = _Node;
  }

  void RemoveNode(TNode *_Node)
  {
    if (_Node->prev)
      _Node->prev->next = _Node->next;

    if (_Node->next)
      _Node->next->prev = _Node->prev;

    if (_Node == m_Head)
      m_Head = m_Head->next;
    else if (_Node == m_Tail)
      m_Tail = m_Tail->prev;

    _Node->next = nullptr;
    _Node->prev = nullptr;
  }

  Key PopLeastRecent()
  {
    const Key ItemRemoved = m_Tail->first;
    RemoveNode(m_Tail);
    return ItemRemoved;
  }

private:
  std::optional<size_t>          m_Capacity;
  std::unordered_map<Key, TNode> m_Cache;

  TNode *m_Head = nullptr;
  TNode *m_Tail = nullptr;
};