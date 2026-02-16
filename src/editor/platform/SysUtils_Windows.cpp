#include "SysUtils.h"
#if DEV_STAGE
#include <Windows.h>
#include <string.h>

namespace utils
{

  std::filesystem::path OpenFileDialog(EFileDialogMode _Mode)
  {
    std::filesystem::path Path;

    if (_Mode == EFileDialogMode::File)
    {
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
    }
    else if (_Mode == EFileDialogMode::Directory)
    {
      BROWSEINFO bi = {0};
      bi.lpszTitle = L"Select a folder";
      LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
      if (pidl != 0)
      {
        TCHAR FolderPath[MAX_PATH];
        if (SHGetPathFromIDList(pidl, FolderPath))
        {
          char FolderPathA[MAX_PATH];
          size_t NumCharConverted;
          wcstombs_s(&NumCharConverted, FolderPathA, MAX_PATH, FolderPath, MAX_PATH);
          Path = FolderPathA;
        }
        CoTaskMemFree(pidl);
      }
    }

    return Path;
  }

}
#endif