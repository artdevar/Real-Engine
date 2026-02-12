#pragma once

#if DEV_STAGE
#include "editor/EditorUI.h"
#endif

#include "Passkey.h"
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
    void SetShadowsEnabled(bool _Enabled, CPasskey<CEditorUI>) { AreShadowsEnabled = _Enabled; }

    void SetCameraZNear(float _ZNear, CPasskey<CEditorUI>) { Camera_ZNear = _ZNear; }
    void SetCameraZFar(float _ZFar, CPasskey<CEditorUI>) { Camera_ZFar = _ZFar; }
    void SetCameraFOV(float _FOV, CPasskey<CEditorUI>) { Camera_FOV = _FOV; }

    void SetLightSpaceMatrixZNear(float _ZNear, CPasskey<CEditorUI>) { LightSpaceMatrix_ZNear = _ZNear; }
    void SetLightSpaceMatrixZFar(float _ZFar, CPasskey<CEditorUI>) { LightSpaceMatrix_ZFar = _ZFar; }
    void SetLightSpaceMatrixOrthLeftBot(float _LeftBot, CPasskey<CEditorUI>) { LightSpaceMatrix_OrthLeftBot = _LeftBot; }
    void SetLightSpaceMatrixOrthRightTop(float _RightTop, CPasskey<CEditorUI>) { LightSpaceMatrix_OrthRightTop = _RightTop; }
#endif
    bool GetShadowsEnabled() const { return AreShadowsEnabled; }

    float GetCameraZNear() const { return Camera_ZNear; }
    float GetCameraZFar() const { return Camera_ZFar; }
    float GetCameraFOV() const { return Camera_FOV; }

    float GetLightSpaceMatrixZNear() const { return LightSpaceMatrix_ZNear; }
    float GetLightSpaceMatrixZFar() const { return LightSpaceMatrix_ZFar; }
    float GetLightSpaceMatrixOrthLeftBot() const { return LightSpaceMatrix_OrthLeftBot; }
    float GetLightSpaceMatrixOrthRightTop() const { return LightSpaceMatrix_OrthRightTop; }

    std::filesystem::path GetAssetsDir() const { return ASSETS_DIR; }
    std::filesystem::path GetTexturesDir() const { return GetAssetsDir() / "textures"; }
    std::filesystem::path GetShadersDir() const { return SHADERS_DIR; }

private:
    // Render
    bool AreShadowsEnabled = true;

    // Camera parameters
    float Camera_ZNear = 0.1f;
    float Camera_ZFar = 1000.0f;
    float Camera_FOV = 60.0f;

    // Light parameters
    float LightSpaceMatrix_ZNear = -20.0f;
    float LightSpaceMatrix_ZFar = 25.0f;
    float LightSpaceMatrix_OrthLeftBot = -30.0f;
    float LightSpaceMatrix_OrthRightTop = 30.0f;
};