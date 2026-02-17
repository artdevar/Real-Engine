#include "SysUtils.h"

#if DEV_STAGE
#include "engine/Config.h"
#include "utils/Common.h"
#include <cstring>

namespace utils
{

std::filesystem::path OpenFileDialog(EFileDialogMode _Mode)
{
  char PathBuffer[256];
  std::memset(PathBuffer, 0x00, sizeof(PathBuffer));

  const std::string AssetsDir = CConfig::Instance().GetAssetsDir().string();

  const std::string_view Title      = (_Mode == EFileDialogMode::SelectFolder) ? "Select a folder" : "Select a file";
  const std::string_view ModeOption = (_Mode == EFileDialogMode::SelectFolder) ? "--directory" : "";
  const std::string_view Template   = "zenity \"$@\" 2>/dev/null --file-selection {} --title=\"{}\" --filename=\"{}\"";

  const std::string Command = std::vformat(Template, std::make_format_args(ModeOption, Title, AssetsDir));

  FILE *File = popen(Command.c_str(), "r");

  fgets(PathBuffer, sizeof(PathBuffer), File);
  pclose(File);

  // Remove trailing newline
  std::string Filename = PathBuffer;
  Filename.erase(Filename.find_last_not_of("\n") + 1);

  return std::filesystem::path(Filename);
}

} // namespace utils
#endif