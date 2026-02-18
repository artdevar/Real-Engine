#pragma once

#include "RenderCommand.h"
#include <vector>

class CRenderQueue
{
public:
  void Push(TRenderCommand _Command)
  {
    m_Commands.push_back(std::move(_Command));
  }

  bool IsEmpty() const
  {
    return m_Commands.empty();
  }

  const std::vector<TRenderCommand> &GetCommands() const
  {
    return m_Commands;
  }

  std::vector<TRenderCommand> StealCommands()
  {
    return std::move(m_Commands);
  }

private:
  std::vector<TRenderCommand> m_Commands;
};