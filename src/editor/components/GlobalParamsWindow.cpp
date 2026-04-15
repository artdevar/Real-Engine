#if DEV_STAGE

#include "GlobalParamsWindow.h"
#include "engine/Config.h"
#include <imgui/imgui.h>
#include <map>

namespace editor
{

void CGlobalParamsWindow::Render()
{
  if (ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
  {
    if (ImGui::CollapsingHeader("Render"))
    {
      int FPSLimit = static_cast<int>(CConfig::Instance().GetFPSLimit());
      if (ImGui::DragInt("FPS limit", &FPSLimit, 5, 0, 1000))
        CConfig::Instance().SetFPSLimit(static_cast<unsigned>(FPSLimit));

      bool ShadowsEnabled = CConfig::Instance().GetShadowsEnabled();
      if (ImGui::Checkbox("Draw shadows", &ShadowsEnabled))
        CConfig::Instance().SetShadowsEnabled(ShadowsEnabled);

      int ShadowMapSize = CConfig::Instance().GetShadowMapSize();
      if (ImGui::DragInt("Shadow map size", &ShadowMapSize, 256, 512, 4096))
        CConfig::Instance().SetShadowsMapSize(ShadowMapSize);
    }

    if (ImGui::CollapsingHeader("Anti Aliasing"))
    {
      static const std::map<int, std::pair<int, const char *>> MSAAItems = []() {
        const int MaxSamples = CConfig::Instance().GetMaxSupportedMSAASamples();

        std::map<int, std::pair<int, const char *>> Items;
        Items[0] = std::make_pair(0, "None");
        if (MaxSamples >= 2)
          Items[2] = std::make_pair(2, "2x");
        if (MaxSamples >= 4)
          Items[4] = std::make_pair(4, "4x");
        if (MaxSamples >= 8)
          Items[8] = std::make_pair(8, "8x");
        if (MaxSamples >= 16)
          Items[16] = std::make_pair(16, "16x");
        return Items;
      }();

      const int MSAASamples = CConfig::Instance().GetMSAASampleCount();
      if (ImGui::BeginCombo("MSAA", MSAAItems.at(MSAASamples).second))
      {
        for (auto it = MSAAItems.cbegin(); it != MSAAItems.cend(); ++it)
        {
          auto &&[Samples, Name] = it->second;

          const bool IsSelected = MSAASamples == Samples;
          if (ImGui::Selectable(Name, IsSelected))
            CConfig::Instance().SetMSAASamples(Samples);
          if (IsSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      ImGui::Separator();

      static const std::map<int, std::pair<int, const char *>> TAAItems = {
          {0, std::make_pair(0, "None")},   //
          {4, std::make_pair(4, "Low")},    //
          {8, std::make_pair(8, "Medium")}, //
          {16, std::make_pair(16, "High")}, //
      };

      const int TAASamples = CConfig::Instance().GetTAASampleCount();
      if (ImGui::BeginCombo("TAA", TAAItems.at(TAASamples).second))
      {
        for (auto it = TAAItems.cbegin(); it != TAAItems.cend(); ++it)
        {
          auto &&[Samples, Name] = it->second;

          const bool IsSelected = TAASamples == Samples;
          if (ImGui::Selectable(Name, IsSelected))
            CConfig::Instance().SetTAASamples(Samples);
          if (IsSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      ImGui::Separator();

      bool FXAAEnabled = CConfig::Instance().GetFXAAEnabled();
      if (ImGui::Checkbox("FXAA", &FXAAEnabled))
        CConfig::Instance().SetFXAAEnabled(FXAAEnabled);
    }

    if (ImGui::CollapsingHeader("Post Process"))
    {
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

      int BloomBlurPasses = CConfig::Instance().GetBloomBlurPasses();
      if (ImGui::DragInt("Bloom blur passes", &BloomBlurPasses, 1, 0, 100))
        CConfig::Instance().SetBloomBlurPasses(BloomBlurPasses);

      ImGui::Separator();

      bool ChromaAberrationEnabled = CConfig::Instance().GetChromaticAberrationEnabled();
      if (ImGui::Checkbox("Chromatic aberration", &ChromaAberrationEnabled))
        CConfig::Instance().SetChromaticAberrationEnabled(ChromaAberrationEnabled);

      TVector3f ChromaAberrationOffset = CConfig::Instance().GetChromaticAberrationOffset();
      if (ImGui::DragFloat3("Chromatic aberration offset", &ChromaAberrationOffset.X, 0.005f, -1.0f, 1.0f))
        CConfig::Instance().SetChromaticAberrationOffset(ChromaAberrationOffset);
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
      if (ImGui::ColorEdit4("Wireframe color", WireframeColorArr, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs))
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
