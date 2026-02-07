#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <format>

CStaticArray<CLogger::LogCallback, CLogger::CallbacksAmount> CLogger::LogCallbacks;
ELogType CLogger::Verbosity = ELogType::Debug;

const std::string_view CLogger::Filename = "engine_log";
const std::map<ELogType, std::string_view> CLogger::MessageType = {
    {ELogType::Debug, "[DEBUG] {}\n"},
    {ELogType::Info, "[INFO] {}\n"},
    {ELogType::Warning, "[WARNING] {}\n"},
    {ELogType::Error, "[ERROR] {}\n"},
    {ELogType::Fatal, "[!] {}\n"}};

void CLogger::DoLog(ELogType _Type, const std::string &_Log)
{
  if (!IsLoggable(_Type))
    return;

  const std::string MessageToLog = std::vformat(MessageType.at(_Type), std::make_format_args(_Log));

  for (auto Callback : LogCallbacks)
    Callback(MessageToLog);
}

bool CLogger::IsLoggable(ELogType _Type)
{
  return static_cast<int>(_Type) >= static_cast<int>(Verbosity);
}

void CLogger::LogToConsole(const std::string &_Log)
{
  std::cout << _Log;
}

void CLogger::LogToFile(const std::string &_Log)
{
  static auto Flags = std::ios::out;

  std::fstream LogFile(Filename.data(), Flags);
  assert(LogFile.is_open());
  Flags = std::ios::app;

  LogFile << _Log;
}
