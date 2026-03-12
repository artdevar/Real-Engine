#include "Path.h"
#include "engine/Config.h"
#include <filesystem>

namespace utils
{
std::vector<std::filesystem::path> GetFilesInDirectory(const std::filesystem::path &_Directory)
{
  std::vector<std::filesystem::path> Files;

  if (std::filesystem::exists(_Directory) && std::filesystem::is_directory(_Directory))
  {
    for (const auto &Entry : std::filesystem::directory_iterator(_Directory))
    {
      if (std::filesystem::is_regular_file(Entry.status()))
        Files.push_back(Entry.path());
    }
  }

  return Files;
}

std::filesystem::path GetRelativePath(const std::filesystem::path &_FullPath)
{
  const auto RelativePath = std::filesystem::relative(_FullPath, CConfig::Instance().GetProjectRoot());
  return !RelativePath.empty() ? RelativePath : _FullPath;
}

} // namespace utils