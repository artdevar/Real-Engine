#include "engine/Engine.h"
#include "utils/Logger.h"

int main(int argc, char *argv[])
{
  CLogger::SetSinks(CLogger::Console | CLogger::File);
  CLogger::SetVerbosity(ELogType::Debug);

  CEngine &Engine = CEngine::Instance();

  if (const int InitCode = Engine.Init(); InitCode != EXIT_SUCCESS)
  {
    CLogger::Log(ELogType::Fatal, "Engine initialisation failed. Error: {}", InitCode);
    Engine.Shutdown();
    return InitCode;
  }

  CLogger::Log(ELogType::Info, "Engine initialisation finished");

  const int RunCode = Engine.Run();
  Engine.Shutdown();

  CLogger::Log(ELogType::Info, "Engine shutdown. Code: {}", RunCode);

  return RunCode;
}