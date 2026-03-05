#if DEV_STAGE

#include "GlobalParamsWindow.h"

#include "engine/Config.h"
#include <imgui/imgui.h>

namespace editor
{

void CGlobalParamsWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    if (ImGui::CollapsingHeader("Render"))
    {
      bool ShadowsEnabled = CConfig::Instance().GetShadowsEnabled();
      if (ImGui::Checkbox("Shadows enabled", &ShadowsEnabled))
        CConfig::Instance().SetShadowsEnabled(ShadowsEnabled);

      int ShadowMapSize = CConfig::Instance().GetShadowMapSize();
      if (ImGui::DragInt("Shadow map size", &ShadowMapSize, 256, 512, 4096))
        CConfig::Instance().SetShadowsMapSize(ShadowMapSize);

      bool FXAAEnabled = CConfig::Instance().GetFXAAEnabled();
      if (ImGui::Checkbox("FXAA enabled", &FXAAEnabled))
        CConfig::Instance().SetFXAAEnabled(FXAAEnabled);

      bool HDREnabled = CConfig::Instance().GetHDREnabled();
      if (ImGui::Checkbox("HDR enabled", &HDREnabled))
        CConfig::Instance().SetHDREnabled(HDREnabled);

      float HDRExposure = CConfig::Instance().GetHDRExposure();
      if (ImGui::DragFloat("HDR exposure", &HDRExposure, 0.1f, 0.0f, 10.0f))
        CConfig::Instance().SetHDRExposure(HDRExposure);

      bool GammaCorrectionEnabled = CConfig::Instance().GetGammaCorrectionEnabled();
      if (ImGui::Checkbox("Gamma correction enabled", &GammaCorrectionEnabled))
        CConfig::Instance().SetGammaCorrectionEnabled(GammaCorrectionEnabled);

      float Gamma = CConfig::Instance().GetGamma();
      if (ImGui::DragFloat("Gamma", &Gamma, 0.1f, 0.01f, 10.0f))
        CConfig::Instance().SetGamma(Gamma);
    }

    if (ImGui::CollapsingHeader("Camera"))
    {
      float CameraZNear = CConfig::Instance().GetCameraZNear();
      if (ImGui::DragFloat("Camera ZNear", &CameraZNear, 0.5f, -50.0f, 100.0f))
        CConfig::Instance().SetCameraZNear(CameraZNear);

      float CameraZFar = CConfig::Instance().GetCameraZFar();
      if (ImGui::DragFloat("Camera ZFar", &CameraZFar, 10.0f, 1.0f, 10000.0f))
        CConfig::Instance().SetCameraZFar(CameraZFar);

      float CameraFOV = CConfig::Instance().GetCameraFOV();
      if (ImGui::DragFloat("Camera FOV", &CameraFOV, 1.0f, 0.0f, 180.0f))
        CConfig::Instance().SetCameraFOV(CameraFOV);
    }

    if (ImGui::CollapsingHeader("Light"))
    {
      float LightZNear = CConfig::Instance().GetLightSpaceMatrixZNear();
      if (ImGui::DragFloat("Light ZNear", &LightZNear, 0.5f, -50.0f, 100.0f))
        CConfig::Instance().SetLightSpaceMatrixZNear(LightZNear);

      float LightZFar = CConfig::Instance().GetLightSpaceMatrixZFar();
      if (ImGui::DragFloat("Light ZFar", &LightZFar, 10.0f, 1.0f, 1000.0f))
        CConfig::Instance().SetLightSpaceMatrixZFar(LightZFar);

      float LightOrthLeftBot = CConfig::Instance().GetLightSpaceMatrixOrthLeftBot();
      if (ImGui::DragFloat("Light Orth Left/Bottom", &LightOrthLeftBot, 1.0f, -100.0f, 0.0f))
        CConfig::Instance().SetLightSpaceMatrixOrthLeftBot(LightOrthLeftBot);

      float LightOrthRightTop = CConfig::Instance().GetLightSpaceMatrixOrthRightTop();
      if (ImGui::DragFloat("Light Orth Right/Top", &LightOrthRightTop, 1.0f, 0.0f, 100.0f))
        CConfig::Instance().SetLightSpaceMatrixOrthRightTop(LightOrthRightTop);
    }

    const ImVec2 Available = ImGui::GetContentRegionAvail();
    m_Size                 = TVector2i(static_cast<int>(Available.x), static_cast<int>(Available.y));
  }

  ImGui::End();
}

} // namespace editor

#endif
