#pragma once

#include <string>
#include <format>
#include <map>
#include "StaticArray.h"

enum class ELogType
{
  Info,
  Warning,
  Error
};

class CLogger final
{
public:

  enum ELogOutput : uint32_t
  {
    None    = 0x00,
    File    = 0x01,
    Console = 0x02
  };

  static constexpr void SetLogOption(uint32_t _OutputOption)
  {
    LogCallbacks.Clear();

    if (_OutputOption & File)
      LogCallbacks.PushBack(CLogger::LogToFile);

    if (_OutputOption & Console)
      LogCallbacks.PushBack(CLogger::LogToConsole);
  }

  template <typename ... Args>
  static void Log(ELogType _Type, std::string_view _Format, Args ... _Args)
  {
    DoLog(_Type, std::vformat(_Format, std::make_format_args(_Args...))); // _Args can't be rvalue
  }

private:

  static void DoLog(ELogType _Type, const std::string & _Log);

  static void LogToConsole(const std::string & _Log);
  static void LogToFile(const std::string & _Log);

private:

  using LogCallback = void(*)(const std::string &);

  static constexpr std::size_t CallbacksAmount = 2;
  static CStaticArray<LogCallback, CallbacksAmount> LogCallbacks;

  static const std::string_view                     Filename;
  static const std::map<ELogType, std::string_view> MessageType;

};