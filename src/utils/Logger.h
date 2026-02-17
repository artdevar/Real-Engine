#pragma once

#include "StaticArray.h"
#include <format>
#include <map>
#include <string>

enum class ELogType
{
  Debug,
  Info,
  Warning,
  Error,
  Fatal
};

class CLogger final
{
public:
  enum ESinks : uint32_t
  {
    None    = 0x00,
    File    = 0x01,
    Console = 0x02
  };

  static constexpr void SetSinks(uint32_t _Sinks)
  {
    LogCallbacks.Clear();

    if (_Sinks & File)
      LogCallbacks.PushBack(CLogger::LogToFile);

    if (_Sinks & Console)
      LogCallbacks.PushBack(CLogger::LogToConsole);
  }

  static constexpr void SetVerbosity(ELogType _Verbosity)
  {
    Verbosity = _Verbosity;
  }

  template <typename... Args>
  static void Log(ELogType _Type, std::string_view _Format, Args... _Args)
  {
    DoLog(_Type, std::vformat(_Format, std::make_format_args(_Args...))); // _Args can't be rvalue
  }

private:
  static void DoLog(ELogType _Type, const std::string &_Log);
  static bool IsLoggable(ELogType _Type);

  static void LogToConsole(const std::string &_Log);
  static void LogToFile(const std::string &_Log);

private:
  using LogCallback = void (*)(const std::string &);

  static constexpr std::size_t                      CallbacksAmount = 2;
  static CStaticArray<LogCallback, CallbacksAmount> LogCallbacks;

  static const std::string_view                     Filename;
  static const std::map<ELogType, std::string_view> MessageType;

  static ELogType Verbosity;
};
