#pragma once

#if DEV_STAGE

#include <filesystem>

namespace utils
{

    std::filesystem::path OpenFileDialog();

}

#endif