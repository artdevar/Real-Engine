#pragma once

#if EDITOR_ENABLED

#include <filesystem>

namespace utils
{

    std::filesystem::path OpenFileDialog();

}

#endif