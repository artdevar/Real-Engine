#pragma once

#include "Logger.h"
#include <chrono>
#include <optional>

class CClock final
{
  DISABLE_CLASS_COPY(CClock);

public:

  CClock() :
    m_Message(),
    m_Start(std::chrono::high_resolution_clock::now())
  {}

  CClock(std::string_view _Message) :
    m_Message(_Message.data()),
    m_Start(std::chrono::high_resolution_clock::now())
  {}

  ~CClock()
  {
    const ClockType End      = std::chrono::high_resolution_clock::now();
    const auto      Duration = std::chrono::duration_cast<std::chrono::milliseconds>(End - m_Start);

    const std::string LogMessage = m_Message.has_value()                     ?
      std::format("[{}] Time execution: {}\n", m_Message.value(), Duration) :
      std::format("Time execution: {}\n", Duration);

    CLogger::Log(ELogType::Info, LogMessage);
  }

private:

  using ClockType = decltype(std::chrono::high_resolution_clock::now());

  std::optional<std::string> m_Message;
  ClockType                  m_Start;

};