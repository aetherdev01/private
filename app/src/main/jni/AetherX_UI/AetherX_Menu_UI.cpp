#include "AetherX_Menu_UI.h"

#include <cstdio>
#include <cstring>

namespace {
const char* BoolText(bool value) { return value ? "Enabled" : "Disabled"; }

void DrawMetric(const char* label, const char* value) {
    ImGui::TextDisabled("%s", label);
    ImGui::SameLine(150.0f);
    ImGui::TextUnformatted(value);
}
}

void AetherXMenuApplyDefaultStyle() {
    ImGui::StyleColorsDark(); // Preserve stock Dear ImGui colors.
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(14.0f, 12.0f);
    style.FramePadding = ImVec2(9.0f, 6.0f);
    style.ItemSpacing = ImVec2(9.0f, 8.0f);
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
}

bool AetherXMenuSaveConfig(const char* configPath,
                           const AetherXMenuSettings& settings) {
    if (configPath == nullptr || configPath[0] == '\0') return false;
    FILE* file = std::fopen(configPath, "w");
    if (file == nullptr) return false;

    const int ok = std::fprintf(
        file,
        "# AetherX UI settings - interface preferences only\n"
        "show_fps=%d\n"
        "show_frame_time=%d\n"
        "compact_spacing=%d\n"
        "show_performance_panel=%d\n"
        "graph_window_seconds=%.2f\n",
        settings.showFps ? 1 : 0,
        settings.showFrameTime ? 1 : 0,
        settings.compactSpacing ? 1 : 0,
        settings.showPerformancePanel ? 1 : 0,
        settings.graphWindowSeconds);

    const int closeResult = std::fclose(file);
    return ok > 0 && closeResult == 0;
}

bool AetherXMenuLoadConfig(const char* configPath,
                           AetherXMenuSettings& settings) {
    if (configPath == nullptr || configPath[0] == '\0') return false;
    FILE* file = std::fopen(configPath, "r");
    if (file == nullptr) return false;

    char line[256];
    while (std::fgets(line, sizeof(line), file) != nullptr) {
        int booleanValue = 0;
        float floatValue = 0.0f;
        if (std::sscanf(line, "show_fps=%d", &booleanValue) == 1) {
            settings.showFps = booleanValue != 0;
        } else if (std::sscanf(line, "show_frame_time=%d", &booleanValue) == 1) {
            settings.showFrameTime = booleanValue != 0;
        } else if (std::sscanf(line, "compact_spacing=%d", &booleanValue) == 1) {
            settings.compactSpacing = booleanValue != 0;
        } else if (std::sscanf(line, "show_performance_panel=%d", &booleanValue) == 1) {
            settings.showPerformancePanel = booleanValue != 0;
        } else if (std::sscanf(line, "graph_window_seconds=%f", &floatValue) == 1) {
            if (floatValue < 3.0f) floatValue = 3.0f;
            if (floatValue > 30.0f) floatValue = 30.0f;
            settings.graphWindowSeconds = floatValue;
        }
    }

    return std::fclose(file) == 0;
}

void AetherXMenuRender(bool* p_open,
                       AetherXMenuSettings& settings,
                       const char* configPath) {
    if (p_open != nullptr && !*p_open) return;

    static char status[128] = "Settings are stored separately from gameplay.";
    static bool didTryInitialLoad = false;
    if (!didTryInitialLoad) {
        if (configPath != nullptr && configPath[0] != '\0') {
            if (AetherXMenuLoadConfig(configPath, settings)) {
                std::snprintf(status, sizeof(status), "Loaded AetherX UI settings.");
            }
        }
        didTryInitialLoad = true;
    }

    ImGui::SetNextWindowSize(ImVec2(590.0f, 470.0f), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("AetherX | Native Menu", p_open, flags)) {
        ImGui::TextUnformatted("AetherX");
        ImGui::SameLine();
        ImGui::TextDisabled("NATIVE UI");
        ImGui::SameLine(ImGui::GetWindowWidth() - 110.0f);
        ImGui::TextDisabled("PUBG LITE 0.27.0");
        ImGui::Separator();

        if (settings.showPerformancePanel && settings.showFps) {
            const ImGuiIO& io = ImGui::GetIO();
            char fpsText[64];
            std::snprintf(fpsText, sizeof(fpsText), "UI FPS: %.0f", io.Framerate);
            ImGui::TextDisabled("%s", fpsText);
            if (settings.showFrameTime && io.Framerate > 0.0f) {
                ImGui::SameLine();
                ImGui::TextDisabled("Frame: %.2f ms", 1000.0f / io.Framerate);
            }
            ImGui::Separator();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
            settings.compactSpacing ? ImVec2(6.0f, 5.0f) : ImVec2(9.0f, 8.0f));

        if (ImGui::BeginTabBar("##AetherXTabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Dashboard")) {
                ImGui::Spacing();
                ImGui::TextUnformatted("Welcome to AetherX");
                ImGui::TextWrapped("Native ImGui interface prototype with a clean tab layout and stock Dear ImGui dark colors.");
                ImGui::Spacing();
                ImGui::Separator();
                DrawMetric("Build target", "Android ARM64");
                DrawMetric("Interface", "Dear ImGui");
                DrawMetric("Configuration", "Local UI preferences");
                ImGui::Spacing();
                ImGui::TextDisabled("This module is UI-only and does not alter game behavior.");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Interface")) {
                ImGui::Spacing();
                ImGui::Checkbox("Compact spacing", &settings.compactSpacing);
                ImGui::Checkbox("Show UI FPS", &settings.showFps);
                ImGui::Checkbox("Show frame time", &settings.showFrameTime);
                ImGui::Checkbox("Show performance header", &settings.showPerformancePanel);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Performance")) {
                ImGui::Spacing();
                const ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("UI render rate: %.1f FPS", io.Framerate);
                ImGui::Text("UI frame time: %.2f ms", io.Framerate > 0.0f ? 1000.0f / io.Framerate : 0.0f);
                ImGui::SliderFloat("Display window", &settings.graphWindowSeconds, 3.0f, 30.0f, "%.0f sec");
                ImGui::TextWrapped("The display window is a saved UI preference for a future telemetry graph; it does not change the game's FPS or performance.");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Config")) {
                ImGui::Spacing();
                ImGui::TextWrapped("Save or reload the interface preferences from the host app's private writable storage.");
                if (ImGui::Button("Save settings", ImVec2(140.0f, 0.0f))) {
                    std::snprintf(status, sizeof(status), "%s",
                        AetherXMenuSaveConfig(configPath, settings)
                            ? "Saved AetherX UI settings."
                            : "Save failed: check the config path.");
                }
                ImGui::SameLine();
                if (ImGui::Button("Load settings", ImVec2(140.0f, 0.0f))) {
                    std::snprintf(status, sizeof(status), "%s",
                        AetherXMenuLoadConfig(configPath, settings)
                            ? "Loaded AetherX UI settings."
                            : "Load failed: file not found or unreadable.");
                }
                ImGui::Separator();
                ImGui::TextWrapped("%s", status);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("About")) {
                ImGui::Spacing();
                ImGui::TextUnformatted("AetherX Native Menu");
                ImGui::Text("Author: AetherX");
                ImGui::Text("UI backend: Dear ImGui 1.85-compatible API");
                ImGui::Separator();
                ImGui::TextWrapped("Designed as a standalone interface component. It contains no aim assistance, player tracking, memory patches, or anti-cheat bypass.");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::PopStyleVar();
    }
    ImGui::End();
}
