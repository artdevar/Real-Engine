#include "engine/Engine.h"
#include <common/Logger.h>

int main(int argc, char *argv[])
{
  CLogger::SetSinks(CLogger::Console | CLogger::File);
  CLogger::SetVerbosity(ELogType::Debug);

  CEngine &Engine = CEngine::Instance();

  if (const int InitCode = Engine.Init(); InitCode != EXIT_SUCCESS)
  {
    LOG_FATAL("Engine initialisation failed. Error: {}", InitCode);
    Engine.Shutdown();
    return InitCode;
  }

  LOG_INFO("Engine initialisation finished");

  const int RunCode = Engine.Run();
  Engine.Shutdown();

  LOG_INFO("Engine shutdown. Code: {}", RunCode);

  return RunCode;
}