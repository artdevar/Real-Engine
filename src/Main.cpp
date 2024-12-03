#include "engine/Engine.h"
#include "utils/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[])
{
  stbi_set_flip_vertically_on_load(true);

  CLogger::SetLogOption(CLogger::Console | CLogger::File);

  CEngine * Engine = CEngine::Instance();

  if (const int InitCode = Engine->Init(); InitCode != EXIT_SUCCESS)
  {
    Engine->Shutdown();
    return InitCode;
  }

  const int RunCode = Engine->Run();
  Engine->Shutdown();

  return RunCode;
}