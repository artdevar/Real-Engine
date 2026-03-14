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
      bool FXAAEnabled = CConfig::Instance().GetFXAAEnabled();
      if (ImGui::Checkbox("FXAA", &FXAAEnabled))
        CConfig::Instance().SetFXAAEnabled(FXAAEnabled);

      ImGui::Separator();

      bool ShadowsEnabled = CConfig::Instance().GetShadowsEnabled();
      if (ImGui::Checkbox("Draw shadows", &ShadowsEnabled))
        CConfig::Instance().SetShadowsEnabled(ShadowsEnabled);

      int ShadowMapSize = CConfig::Instance().GetShadowMapSize();
      if (ImGui::DragInt("Shadow map size", &ShadowMapSize, 256, 512, 4096))
        CConfig::Instance().SetShadowsMapSize(ShadowMapSize);

      ImGui::Separator();

      bool HDREnabled = CConfig::Instance().GetHDREnabled();
      if (ImGui::Checkbox("HDR", &HDREnabled))
        CConfig::Instance().SetHDREnabled(HDREnabled);

      float HDRExposure = CConfig::Instance().GetHDRExposure();
      if (ImGui::DragFloat("HDR exposure", &HDRExposure, 0.1f, 0.0f, 10.0f))
        CConfig::Instance().SetHDRExposure(HDRExposure);

      ImGui::Separator();

      bool GammaCorrectionEnabled = CConfig::Instance().GetGammaCorrectionEnabled();
      if (ImGui::Checkbox("Gamma correction", &GammaCorrectionEnabled))
        CConfig::Instance().SetGammaCorrectionEnabled(GammaCorrectionEnabled);

      float Gamma = CConfig::Instance().GetGamma();
      if (ImGui::DragFloat("Gamma", &Gamma, 0.1f, 0.01f, 10.0f))
        CConfig::Instance().SetGamma(Gamma);

      ImGui::Separator();

      bool BloomEnabled = CConfig::Instance().GetBloomEnabled();
      if (ImGui::Checkbox("Bloom", &BloomEnabled))
        CConfig::Instance().SetBloomEnabled(BloomEnabled);

      float BloomThreshold = CConfig::Instance().GetBloomThreshold();
      if (ImGui::DragFloat("Bloom threshold", &BloomThreshold, 0.1f, 0.0f, 10.0f))
        CConfig::Instance().SetBloomThreshold(BloomThreshold);

      float BloomIntensity = CConfig::Instance().GetBloomIntensity();
      if (ImGui::DragFloat("Bloom intensity", &BloomIntensity, 0.1f, 0.0f, 10.0f))
        CConfig::Instance().SetBloomIntensity(BloomIntensity);
    }

    if (ImGui::CollapsingHeader("Debug"))
    {
      bool GizmoEnabled = CConfig::Instance().GetGizmoEnabled();
      if (ImGui::Checkbox("Draw gizmo", &GizmoEnabled))
        CConfig::Instance().SetGizmoEnabled(GizmoEnabled);

      ImGui::Separator();

      bool GridEnabled = CConfig::Instance().GetGridEnabled();
      if (ImGui::Checkbox("Draw grid", &GridEnabled))
        CConfig::Instance().SetGridEnabled(GridEnabled);

      ImGui::Separator();

      bool WireframeEnabled = CConfig::Instance().GetWireframeEnabled();
      if (ImGui::Checkbox("Draw wireframe", &WireframeEnabled))
        CConfig::Instance().SetWireframeEnabled(WireframeEnabled);

      TColor WireframeColor       = CConfig::Instance().GetWireframeColor();
      float  WireframeColorArr[4] = {WireframeColor.R, WireframeColor.G, WireframeColor.B, WireframeColor.A};
      if (ImGui::ColorEdit4("Wireframe color", WireframeColorArr, ImGuiColorEditFlags_Float))
        CConfig::Instance().SetWireframeColor(TColor(WireframeColorArr[0], WireframeColorArr[1], WireframeColorArr[2], WireframeColorArr[3]));
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
    m_Size                 = TVector2i(Available.x, Available.y);
  }

  ImGui::End();
}

} // namespace editor

#endif
