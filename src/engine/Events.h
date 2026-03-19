#pragma once

enum class TEventType
{
  WindowResized = 0,
  RequestAppShutdown,
  ViewportResized,
  EntityRemoved,

  Config_CameraFOVChanged = 1000,
  Config_CameraZNearChanged,
  Config_CameraZFarChanged,
  Config_ShadowsEnabledChanged,
  Config_ShadowsMapSizeChanged,
  Config_FXAAEnabledChanged,
  Config_TAAEnabledChanged,
  Config_MSAASamplesChanged,
  Config_TAAJitterSampleCountChanged,
  Config_HDREnabledChanged,
  Config_HDRExposureChanged,
  Config_GammaCorrectionEnabledChanged,
  Config_GammaChanged,
  Config_SSAOEnabledChanged,
  Config_GizmoEnabledChanged,
  Config_GridEnabledChanged,
  Config_WireframeEnabledChanged,
  Config_WireframeColorChanged,
  Config_BloomEnabledChanged,
  Config_BloomThresholdChanged,
  Config_BloomIntensityChanged,
  Config_BloomBlurPassesChanged,
  Config_LightSpaceMatrixZNearChanged,
  Config_LightSpaceMatrixZFarChanged,
  Config_LightSpaceMatrixOrthLeftBotChanged,
  Config_LightSpaceMatrixOrthRightTopChanged,

  Editor_RequestAppClose = 2000,
  Editor_EntitySelected,
  Editor_EntityDeselected
};
