#include "engine/Engine.h"
#include "utils/Logger.h"
#include "stb_image.h"

#define TINYGLTF_IMPLEMENTATION

int main(int argc, char *argv[])
{
  stbi_set_flip_vertically_on_load(false);

  CLogger::SetSinks(CLogger::Console | CLogger::File);

  CEngine &Engine = CEngine::Instance();

  if (const int InitCode = Engine.Init("", "Real Engine"); InitCode != EXIT_SUCCESS)
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