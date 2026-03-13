#pragma once

#include <chrono>

namespace utils
{

class CClock final
{
public:
  using ClockType    = std::chrono::high_resolution_clock;
  using TimePoint    = std::chrono::time_point<ClockType>;
  using Duration     = std::chrono::duration<float>;
  using Milliseconds = std::chrono::duration<float, std::milli>;

public:
  CClock() :
      m_StartTime(ClockType::now())
  {
  }

  float Restart()
  {
    const TimePoint Now     = ClockType::now();
    const float     Elapsed = std::chrono::duration_cast<Duration>(Now - m_StartTime).count();
    m_StartTime             = Now;
    return Elapsed;
  }

  float GetElapsedTime() const
  {
    return std::chrono::duration_cast<Duration>(ClockType::now() - m_StartTime).count();
  }

  float GetElapsedTimeMs() const
  {
    return std::chrono::duration_cast<Milliseconds>(ClockType::now() - m_StartTime).count();
  }

  static TimePoint Now()
  {
    return ClockType::now();
  }

private:
  TimePoint m_StartTime;
};

} // namespace utils
