#include "SysUtils.h"
#if ENABLE_EDITOR
#include <Windows.h>
#include <string.h>

namespace utils
{

  std::filesystem::path OpenFileDialog()
  {
    std::filesystem::path Path;

    TCHAR FilenameWide[MAX_PATH];
    char Filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(FilenameWide, sizeof(FilenameWide));
    ZeroMemory(Filename, sizeof(Filename));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = FilenameWide;
    ofn.nMaxFile = sizeof(FilenameWide);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
      size_t NumCharConverted;
      wcstombs_s(&NumCharConverted, Filename, MAX_PATH, FilenameWide, MAX_PATH);
      Path = Filename;
    }

    return Path;
  }

}
#endif