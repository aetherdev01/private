#pragma once

#include "imgui.h"

// Standalone preview state. These controls are for UI preview only and do not
// connect to a game process or modify gameplay.
struct AetherXMenuSettings {
    bool showFps = true;
    bool showFrameTime = true;
    bool compactSpacing = false;
    bool showPerformancePanel = true;
    float graphWindowSeconds = 10.0f;
};

void AetherXMenuApplyDefaultStyle();
void AetherXMenuRender(bool* p_open,
                       AetherXMenuSettings& settings,
                       const char* configPath);
bool AetherXMenuSaveConfig(const char* configPath,
                           const AetherXMenuSettings& settings);
bool AetherXMenuLoadConfig(const char* configPath,
                           AetherXMenuSettings& settings);
