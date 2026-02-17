#pragma once

#if DEV_STAGE

#include <filesystem>

namespace utils
{
enum class EFileDialogMode
{
  SelectFile,
  SelectFolder
};

std::filesystem::path OpenFileDialog(EFileDialogMode _Mode);

} // namespace utils

#endif