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
  void SetTAAEnabled(bool _Enabled)
  {
    if (IsTAAEnabled != _Enabled)
    {
      IsTAAEnabled = _Enabled;
      event::Notify(TEventType::Config_TAAEnabledChanged, _Enabled);
    }
  }
  void SetTAAJitterSampleCount(int _Count)
  {
    if (TAAJitterSampleCount != _Count)
    {
      TAAJitterSampleCount = _Count;
      event::Notify(TEventType::Config_TAAJitterSampleCountChanged, _Count);
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
  void SetSSAOEnabled(bool _Enabled)
  {
    if (IsSSAOEnabled != _Enabled)
    {
      IsSSAOEnabled = _Enabled;
      event::Notify(TEventType::Config_SSAOEnabledChanged, _Enabled);
    }
  }
  void SetGizmoEnabled(bool _Enabled)
  {
    if (IsGizmoEnabled != _Enabled)
    {
      IsGizmoEnabled = _Enabled;
      event::Notify(TEventType::Config_GizmoEnabledChanged, _Enabled);
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
  void SetBloomEnabled(bool _Enabled)
  {
    if (IsBloomEnabled != _Enabled)
    {
      IsBloomEnabled = _Enabled;
      event::Notify(TEventType::Config_BloomEnabledChanged, _Enabled);
    }
  }
  void SetBloomThreshold(float _Threshold)
  {
    if (!math::AreEqual(BloomThreshold, _Threshold))
    {
      BloomThreshold = _Threshold;
      event::Notify(TEventType::Config_BloomThresholdChanged, _Threshold);
    }
  }
  void SetBloomIntensity(float _Intensity)
  {
    if (!math::AreEqual(BloomIntensity, _Intensity))
    {
      BloomIntensity = _Intensity;
      event::Notify(TEventType::Config_BloomIntensityChanged, _Intensity);
    }
  }
  void SetBloomBlurPasses(int _Passes)
  {
    if (BloomBlurPasses != _Passes)
    {
      BloomBlurPasses = _Passes;
      event::Notify(TEventType::Config_BloomBlurPassesChanged, _Passes);
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
  bool GetTAAEnabled() const
  {
    return IsTAAEnabled;
  }
  int GetTAAJitterSampleCount() const
  {
    return TAAJitterSampleCount;
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
  bool GetBloomEnabled() const
  {
    return IsBloomEnabled;
  }
  float GetBloomThreshold() const
  {
    return BloomThreshold;
  }
  float GetBloomIntensity() const
  {
    return BloomIntensity;
  }
  int GetBloomBlurPasses() const
  {
    return BloomBlurPasses;
  }
  float GetGamma() const
  {
    return Gamma;
  }
  bool GetSSAOEnabled() const
  {
    return IsSSAOEnabled;
  }

  bool GetGizmoEnabled() const
  {
    return IsGizmoEnabled;
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

  std::string GetAppTitle() const
  {
    return APP_NAME;
  }
  std::filesystem::path GetAppIconPath() const
  {
    return APP_ICON;
  }
  std::filesystem::path GetProjectRoot() const
  {
    return PROJECT_ROOT;
  }
  std::filesystem::path GetShadersDir() const
  {
    return SHADERS_DIR;
  }
  std::filesystem::path GetAssetsDir() const
  {
    return ASSETS_DIR;
  }
  std::filesystem::path GetFontsDir() const
  {
    return GetAssetsDir() / "fonts";
  }
  std::filesystem::path GetTexturesDir() const
  {
    return GetAssetsDir() / "textures";
  }
  bool IsEditorEnabled() const
  {
    return static_cast<bool>(DEV_STAGE);
  }

private:
  // Render
  int   ShadowMapSize            = 4096;
  bool  AreShadowsEnabled        = true;
  bool  IsFXAAEnabled            = false;
  bool  IsTAAEnabled             = true;
  int   TAAJitterSampleCount     = 8;
  bool  IsHDREnabled             = true;
  float HDRExposure              = 1.0f;
  bool  IsGammaCorrectionEnabled = true;
  float Gamma                    = 2.2f;
  bool  IsBloomEnabled           = true;
  float BloomThreshold           = 1.0f;
  float BloomIntensity           = 1.0f;
  int   BloomBlurPasses          = 10;
  bool  IsSSAOEnabled            = true;

  // Debug
  bool   IsGizmoEnabled     = true;
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
