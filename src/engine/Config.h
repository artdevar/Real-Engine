#pragma once

#if DEV_STAGE
#include "editor/EditorUI.h"
#endif

#include "Passkey.h"
#include "utils/Event.h"
#include "engine/MathCore.h"
#include <filesystem>

class CConfig final
{
public:
  static CConfig &Instance()
  {
    static CConfig Instance;
    return Instance;
  }

#if DEV_STAGE
  void SetShadowsEnabled(bool _Enabled, CPasskey<CEditorUI>)
  {
    if (AreShadowsEnabled != _Enabled)
    {
      AreShadowsEnabled = _Enabled;
      event::Notify(TEventType::Config_ShadowsEnabledChanged, _Enabled);
    }
  }
  void SetFXAAEnabled(bool _Enabled, CPasskey<CEditorUI>)
  {
    if (IsFXAAEnabled != _Enabled)
    {
      IsFXAAEnabled = _Enabled;
      event::Notify(TEventType::Config_FXAAEnabledChanged, _Enabled);
    }
  }
  void SetHDREnabled(bool _Enabled, CPasskey<CEditorUI>)
  {
    if (IsHDREnabled != _Enabled)
    {
      IsHDREnabled = _Enabled;
      event::Notify(TEventType::Config_HDREnabledChanged, _Enabled);
    }
  }
  void SetHDRExposure(float _Exposure, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(HDRExposure, _Exposure))
    {
      HDRExposure = _Exposure;
      event::Notify(TEventType::Config_HDRExposureChanged, _Exposure);
    }
  }

  void SetCameraZNear(float _ZNear, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(Camera_ZNear, _ZNear))
    {
      Camera_ZNear = _ZNear;
      event::Notify(TEventType::Config_CameraZNearChanged, _ZNear);
    }
  }
  void SetCameraZFar(float _ZFar, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(Camera_ZFar, _ZFar))
    {
      Camera_ZFar = _ZFar;
      event::Notify(TEventType::Config_CameraZFarChanged, _ZFar);
    }
  }
  void SetCameraFOV(float _FOV, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(Camera_FOV, _FOV))
    {
      Camera_FOV = _FOV;
      event::Notify(TEventType::Config_CameraFOVChanged, _FOV);
    }
  }

  void SetLightSpaceMatrixZNear(float _ZNear, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(LightSpaceMatrix_ZNear, _ZNear))
    {
      LightSpaceMatrix_ZNear = _ZNear;
      event::Notify(TEventType::Config_LightSpaceMatrixZNearChanged, _ZNear);
    }
  }
  void SetLightSpaceMatrixZFar(float _ZFar, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(LightSpaceMatrix_ZFar, _ZFar))
    {
      LightSpaceMatrix_ZFar = _ZFar;
      event::Notify(TEventType::Config_LightSpaceMatrixZFarChanged, _ZFar);
    }
  }
  void SetLightSpaceMatrixOrthLeftBot(float _LeftBot, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(LightSpaceMatrix_OrthLeftBot, _LeftBot))
    {
      LightSpaceMatrix_OrthLeftBot = _LeftBot;
      event::Notify(TEventType::Config_LightSpaceMatrixOrthLeftBotChanged, _LeftBot);
    }
  }
  void SetLightSpaceMatrixOrthRightTop(float _RightTop, CPasskey<CEditorUI>)
  {
    if (!math::AreEqual(LightSpaceMatrix_OrthRightTop, _RightTop))
    {
      LightSpaceMatrix_OrthRightTop = _RightTop;
      event::Notify(TEventType::Config_LightSpaceMatrixOrthRightTopChanged, _RightTop);
    }
  }
#endif
  std::string GetAppTitle() const
  {
    return APP_NAME;
  }

  int GetShadowMapSize() const
  {
    return ShadowMapSize;
  }
  bool GetShadowsEnabled() const
  {
    return AreShadowsEnabled;
  }
  bool GetFXAAEnabled() const
  {
    return IsFXAAEnabled;
  }
  bool GetHDREnabled() const
  {
    return IsHDREnabled;
  }
  float GetHDRExposure() const
  {
    return HDRExposure;
  }

  float GetCameraZNear() const
  {
    return Camera_ZNear;
  }
  float GetCameraZFar() const
  {
    return Camera_ZFar;
  }
  float GetCameraFOV() const
  {
    return Camera_FOV;
  }

  float GetLightSpaceMatrixZNear() const
  {
    return LightSpaceMatrix_ZNear;
  }
  float GetLightSpaceMatrixZFar() const
  {
    return LightSpaceMatrix_ZFar;
  }
  float GetLightSpaceMatrixOrthLeftBot() const
  {
    return LightSpaceMatrix_OrthLeftBot;
  }
  float GetLightSpaceMatrixOrthRightTop() const
  {
    return LightSpaceMatrix_OrthRightTop;
  }

  std::filesystem::path GetShadersDir() const
  {
    return SHADERS_DIR;
  }
  std::filesystem::path GetAssetsDir() const
  {
    return ASSETS_DIR;
  }
  std::filesystem::path GetTexturesDir() const
  {
    return GetAssetsDir() / "textures";
  }

private:
  // Render
  int   ShadowMapSize     = 2048;
  bool  AreShadowsEnabled = true;
  bool  IsFXAAEnabled     = true;
  bool  IsHDREnabled      = true;
  float HDRExposure       = 1.0f;

  // Camera parameters
  float Camera_ZNear = 0.1f;
  float Camera_ZFar  = 1000.0f;
  float Camera_FOV   = 60.0f;

  // Light parameters
  float LightSpaceMatrix_ZNear        = -20.0f;
  float LightSpaceMatrix_ZFar         = 45.0f;
  float LightSpaceMatrix_OrthLeftBot  = -30.0f;
  float LightSpaceMatrix_OrthRightTop = 30.0f;
};