#include "SysUtils.h"
#if DEV_STAGE
#include <Windows.h>
#include <shlobj.h>
#include <string.h>

namespace utils
{

std::filesystem::path OpenFileDialog(EFileDialogMode _Mode)
{
  const _FILEOPENDIALOGOPTIONS Option = _Mode == EFileDialogMode::SelectFile ? FOS_FILEMUSTEXIST :
    _Mode == EFileDialogMode::SelectFolder ? FOS_PICKFOLDERS : FOS_FILEMUSTEXIST;

  IFileDialog * pfd = nullptr;

  HRESULT hr = CoCreateInstance(
    CLSID_FileOpenDialog,
    nullptr,
    CLSCTX_ALL,
    IID_PPV_ARGS(&pfd)
  );

  if (FAILED(hr))
    return {};

  DWORD options;
  pfd->GetOptions(&options);
  pfd->SetOptions(options | Option);

  hr = pfd->Show(nullptr);
  if (FAILED(hr))
  {
    pfd->Release();
    return {};
  }

  IShellItem * psi = nullptr;
  hr = pfd->GetResult(&psi);
  if (FAILED(hr))
  {
    pfd->Release();
    return {};
  }

  PWSTR path = nullptr;
  psi->GetDisplayName(SIGDN_FILESYSPATH, &path);

  std::filesystem::path result(path);

  CoTaskMemFree(path);
  psi->Release();
  pfd->Release();

  return result;
}

}
#endif