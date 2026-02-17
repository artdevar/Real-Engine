#pragma once

#include <filesystem>
#include <vector>

namespace utils
{
std::vector<std::filesystem::path> GetFilesInDirectory(const std::filesystem::path &_Directory);
}