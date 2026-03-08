#pragma once

#include "utils/Event.h"
#include <common/MathCore.h>
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
  void SetShadowsEnabled(bool _Enabled)
  {
    if (AreShadowsEnabled != _Enabled)
    {
      AreShadowsEnabled = _Enabled;
      event::Notify(TEventType::Config_ShadowsEnabledChanged, _Enabled);
    }
  }
  void SetShadowsMapSize(int _Size)
  {
    if (ShadowMapSize != _Size)
    {
      ShadowMapSize = _Size;
      event::Notify(TEventType::Config_ShadowsMapSizeChanged, _Size);
    }
  }
  void SetFXAAEnabled(bool _Enabled)
  {
    if (IsFXAAEnabled != _Enabled)
    {
      IsFXAAEnabled = _Enabled;
      event::Notify(TEventType::Config_FXAAEnabledChanged, _Enabled);
    }
  }
  void SetHDREnabled(bool _Enabled)
  {
    if (IsHDREnabled != _Enabled)
    {
      IsHDREnabled = _Enabled;
      event::Notify(TEventType::Config_HDREnabledChanged, _Enabled);
    }
  }
  void SetHDRExposure(float _Exposure)
  {
    if (!math::AreEqual(HDRExposure, _Exposure))
    {
      HDRExposure = _Exposure;
      event::Notify(TEventType::Config_HDRExposureChanged, _Exposure);
    }
  }
  void SetGammaCorrectionEnabled(bool _Enabled)
  {
    if (IsGammaCorrectionEnabled != _Enabled)
    {
      IsGammaCorrectionEnabled = _Enabled;
      event::Notify(TEventType::Config_GammaCorrectionEnabledChanged, _Enabled);
    }
  }
  void SetGamma(float _Gamma)
  {
    if (!math::AreEqual(Gamma, _Gamma))
    {
      Gamma = _Gamma;
      event::Notify(TEventType::Config_GammaChanged, _Gamma);
    }
  }
  void SetGridEnabled(bool _Enabled)
  {
    if (IsGridEnabled != _Enabled)
    {
      IsGridEnabled = _Enabled;
      event::Notify(TEventType::Config_GridEnabledChanged, _Enabled);
    }
  }
  void SetWireframeEnabled(bool _Enabled)
  {
    if (IsWireframeEnabled != _Enabled)
    {
      IsWireframeEnabled = _Enabled;
      event::Notify(TEventType::Config_WireframeEnabledChanged, _Enabled);
    }
  }
  void SetWireframeColor(const TColor &_Color)
  {
    if (WireframeColor != _Color)
    {
      WireframeColor = _Color;
      event::Notify(TEventType::Config_WireframeColorChanged, _Color);
    }
  }

  void SetCameraZNear(float _ZNear)
  {
    if (!math::AreEqual(Camera_ZNear, _ZNear))
    {
      Camera_ZNear = _ZNear;
      event::Notify(TEventType::Config_CameraZNearChanged, _ZNear);
    }
  }
  void SetCameraZFar(float _ZFar)
  {
    if (!math::AreEqual(Camera_ZFar, _ZFar))
    {
      Camera_ZFar = _ZFar;
      event::Notify(TEventType::Config_CameraZFarChanged, _ZFar);
    }
  }
  void SetCameraFOV(float _FOV)
  {
    if (!math::AreEqual(Camera_FOV, _FOV))
    {
      Camera_FOV = _FOV;
      event::Notify(TEventType::Config_CameraFOVChanged, _FOV);
    }
  }

  void SetLightSpaceMatrixZNear(float _ZNear)
  {
    if (!math::AreEqual(LightSpaceMatrix_ZNear, _ZNear))
    {
      LightSpaceMatrix_ZNear = _ZNear;
      event::Notify(TEventType::Config_LightSpaceMatrixZNearChanged, _ZNear);
    }
  }
  void SetLightSpaceMatrixZFar(float _ZFar)
  {
    if (!math::AreEqual(LightSpaceMatrix_ZFar, _ZFar))
    {
      LightSpaceMatrix_ZFar = _ZFar;
      event::Notify(TEventType::Config_LightSpaceMatrixZFarChanged, _ZFar);
    }
  }
  void SetLightSpaceMatrixOrthLeftBot(float _LeftBot)
  {
    if (!math::AreEqual(LightSpaceMatrix_OrthLeftBot, _LeftBot))
    {
      LightSpaceMatrix_OrthLeftBot = _LeftBot;
      event::Notify(TEventType::Config_LightSpaceMatrixOrthLeftBotChanged, _LeftBot);
    }
  }
  void SetLightSpaceMatrixOrthRightTop(float _RightTop)
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
  bool GetGammaCorrectionEnabled() const
  {
    return IsGammaCorrectionEnabled;
  }
  float GetGamma() const
  {
    return Gamma;
  }
  bool GetGridEnabled() const
  {
    return IsGridEnabled;
  }
  bool GetWireframeEnabled() const
  {
    return IsWireframeEnabled;
  }
  TColor GetWireframeColor() const
  {
    return WireframeColor;
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

  constexpr std::filesystem::path GetShadersDir() const
  {
    return SHADERS_DIR;
  }
  constexpr std::filesystem::path GetAssetsDir() const
  {
    return ASSETS_DIR;
  }
  constexpr std::filesystem::path GetFontsDir() const
  {
    return GetAssetsDir() / "fonts";
  }
  constexpr std::filesystem::path GetTexturesDir() const
  {
    return GetAssetsDir() / "textures";
  }
  constexpr bool IsEditorEnabled() const
  {
    return static_cast<bool>(DEV_STAGE);
  }

private:
  // Render
  int   ShadowMapSize            = 4096;
  bool  AreShadowsEnabled        = true;
  bool  IsFXAAEnabled            = true;
  bool  IsHDREnabled             = false;
  float HDRExposure              = 1.0f;
  bool  IsGammaCorrectionEnabled = false;
  float Gamma                    = 2.2f;

  // Debug
  bool   IsGridEnabled      = true;
  bool   IsWireframeEnabled = true;
  TColor WireframeColor     = TColor(1.0f, 1.0f, 0.0f, 0.8f);

  // Camera parameters
  float Camera_ZNear = 0.1f;
  float Camera_ZFar  = 1000.0f;
  float Camera_FOV   = 60.0f;

  // Light parameters
  float LightSpaceMatrix_ZNear        = -45.0f;
  float LightSpaceMatrix_ZFar         = 45.0f;
  float LightSpaceMatrix_OrthLeftBot  = -30.0f;
  float LightSpaceMatrix_OrthRightTop = 30.0f;
};