#pragma once

#include "imgui.h"

// Standalone AetherX menu UI. These settings affect only this UI prototype;
// they are intentionally not connected to game memory, aiming, or hooks.
struct AetherXMenuSettings {
    bool showFps = true;
    bool showFrameTime = true;
    bool compactSpacing = false;
    bool showPerformancePanel = true;
    float graphWindowSeconds = 10.0f;
};

// Call once after ImGui::CreateContext(). The palette remains Dear ImGui's
// standard dark palette; only layout/rounding metrics are adjusted slightly.
void AetherXMenuApplyDefaultStyle();

// Call during each ImGui frame after ImGui::NewFrame().
// configPath must point to a writable file in the host app's own storage.
void AetherXMenuRender(bool* p_open,
                       AetherXMenuSettings& settings,
                       const char* configPath);

bool AetherXMenuSaveConfig(const char* configPath,
                           const AetherXMenuSettings& settings);
bool AetherXMenuLoadConfig(const char* configPath,
                           AetherXMenuSettings& settings);
