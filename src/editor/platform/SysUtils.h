#pragma once

#if ENABLE_EDITOR

#include <filesystem>

namespace utils
{

    std::filesystem::path OpenFileDialog();

}

#endif