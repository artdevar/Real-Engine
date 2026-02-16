#pragma once

#include <vector>
#include <filesystem>

namespace utils
{
    std::vector<std::filesystem::path> GetFilesInDirectory(const std::filesystem::path &_Directory);
}