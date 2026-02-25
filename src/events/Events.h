#pragma once

enum class TEventType
{
  Config_CameraFOVChanged = 1000,
  Config_CameraZNearChanged,
  Config_CameraZFarChanged,
  Config_ShadowsEnabledChanged,
  Config_FXAAEnabledChanged,
  Config_HDREnabledChanged,
  Config_HDRExposureChanged,
  Config_LightSpaceMatrixZNearChanged,
  Config_LightSpaceMatrixZFarChanged,
  Config_LightSpaceMatrixOrthLeftBotChanged,
  Config_LightSpaceMatrixOrthRightTopChanged,
};