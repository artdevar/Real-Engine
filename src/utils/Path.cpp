#include "Path.h"

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
}