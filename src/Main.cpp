#include "engine/Engine.h"
#include "utils/Logger.h"
#include "stb_image.h"

#define TINYGLTF_IMPLEMENTATION

int main(int argc, char *argv[])
{
  stbi_set_flip_vertically_on_load(true);

  CLogger::SetLogOption(CLogger::Console | CLogger::File);

  CEngine * Engine = CEngine::Instance();

  if (const int InitCode = Engine->Init(); InitCode != EXIT_SUCCESS)
  {
    CLogger::Log(ELogType::Error, "Engine initialisation failed. Error: {}", InitCode);
    Engine->Shutdown();
    return InitCode;
  }

  CLogger::Log(ELogType::Info, "Engine initialisation finished\n");

  const int RunCode = Engine->Run();
  Engine->Shutdown();

  return RunCode;
}