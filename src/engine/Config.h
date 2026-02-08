#pragma once

#include "Passkey.h"

#if EDITOR_ENABLED
#include "editor/EditorUI.h"
#endif

class CConfig final
{
public:
    static CConfig &Instance()
    {
        static CConfig Instance;
        return Instance;
    }

#if EDITOR_ENABLED
    void SetCameraZNear(float _ZNear, CPasskey<CEditorUI>) { Camera_ZNear = _ZNear; }
    void SetCameraZFar(float _ZFar, CPasskey<CEditorUI>) { Camera_ZFar = _ZFar; }
    void SetCameraFOV(float _FOV, CPasskey<CEditorUI>) { Camera_FOV = _FOV; }

    void SetLightSpaceMatrixZNear(float _ZNear, CPasskey<CEditorUI>) { LightSpaceMatrix_ZNear = _ZNear; }
    void SetLightSpaceMatrixZFar(float _ZFar, CPasskey<CEditorUI>) { LightSpaceMatrix_ZFar = _ZFar; }
#endif
    float GetCameraZNear() const { return Camera_ZNear; }
    float GetCameraZFar() const { return Camera_ZFar; }
    float GetCameraFOV() const { return Camera_FOV; }

    float GetLightSpaceMatrixZNear() const { return LightSpaceMatrix_ZNear; }
    float GetLightSpaceMatrixZFar() const { return LightSpaceMatrix_ZFar; }

private:
    // Camera parameters
    float Camera_ZNear = 0.1f;
    float Camera_ZFar = 1000.0f;
    float Camera_FOV = 60.0f;

    // Light parameters
    float LightSpaceMatrix_ZNear = 1.0f;
    float LightSpaceMatrix_ZFar = 20.0f;
};