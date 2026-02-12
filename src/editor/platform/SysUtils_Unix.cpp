#include "SysUtils.h"

#if DEV_STAGE
#include "utils/Common.h"
#include <cstring>

namespace utils
{

  std::filesystem::path OpenFileDialog()
  {
    char PathBuffer[256];
    std::memset(PathBuffer, 0x00, sizeof(PathBuffer));

    FILE *File = popen("zenity \"$@\" 2>/dev/null --file-selection --title=\"Select a file\"", "r");

    fgets(PathBuffer, sizeof(PathBuffer), File);
    pclose(File);

    // Remove trailing newline
    std::string Filename = PathBuffer;
    Filename.erase(Filename.find_last_not_of("\n") + 1);

    return std::filesystem::path(Filename);
  }

}
#endif