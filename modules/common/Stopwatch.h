#pragma once

#include "Core.h"
#include "Logger.h"
#include <chrono>
#include <optional>
#include <format>

namespace utils
{

class CStopwatch final
{
  DISABLE_CLASS_COPY(CStopwatch);

public:
  CStopwatch() :
      m_Message(),
      m_Start(std::chrono::high_resolution_clock::now())
  {
  }

  CStopwatch(std::string_view _Message) :
      m_Message(_Message.data()),
      m_Start(std::chrono::high_resolution_clock::now())
  {
  }

  ~CStopwatch()
  {
    const ClockType End      = std::chrono::high_resolution_clock::now();
    const auto      Duration = std::chrono::duration_cast<std::chrono::milliseconds>(End - m_Start);

    const std::string LogMessage =
        m_Message.has_value() ? std::format("[{}] Time execution: {}", m_Message.value(), Duration) : std::format("Time execution: {}", Duration);

    CLogger::Log(ELogType::Info, LogMessage);
  }

private:
  using ClockType = decltype(std::chrono::high_resolution_clock::now());

  std::optional<std::string> m_Message;
  ClockType                  m_Start;
};

} // namespace utils

#if DEV_STAGE
#define MEASURE_ZONE(Message) utils::CStopwatch stopwatch_##__LINE__(Message);
#else
#define MEASURE_ZONE(Message) void(0);
#endif