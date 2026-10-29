#include "AetherX_Menu_UI.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace {
void DrawMetric(const char* label, const char* value) {
    ImGui::TextDisabled("%s", label);
    ImGui::SameLine(150.0f);
    ImGui::TextUnformatted(value);
}
}

void AetherXMenuApplyDefaultStyle() {
    // Use Dear ImGui's stock palette without overriding any color entries.
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(12.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 7.0f);
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
    const int result = std::fprintf(
        file,
        "# RAMBO ImGui preview preferences only\n"
        "show_fps=%d\nshow_frame_time=%d\ncompact_spacing=%d\n"
        "show_performance_panel=%d\ngraph_window_seconds=%.2f\n",
        settings.showFps ? 1 : 0,
        settings.showFrameTime ? 1 : 0,
        settings.compactSpacing ? 1 : 0,
        settings.showPerformancePanel ? 1 : 0,
        settings.graphWindowSeconds);
    const int closeResult = std::fclose(file);
    return result > 0 && closeResult == 0;
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
            settings.graphWindowSeconds = std::max(3.0f, std::min(30.0f, floatValue));
        }
    }
    return std::fclose(file) == 0;
}

void AetherXMenuRender(bool* p_open,
                       AetherXMenuSettings& settings,
                       const char* configPath) {
    if (p_open != nullptr && !*p_open) return;
    static bool didTryInitialLoad = false;
    static char status[128] = "Preview ready. Controls do not modify gameplay.";
    static bool demoAim = false;
    static bool demoAutoFire = false;
    static bool demoEsp = true;
    static bool demoBox = true;
    static bool demoSkeleton = false;
    static bool demoNames = true;
    static bool demoLoot = false;
    static bool demoParachute = false;
    static bool demoAmmo = false;
    static bool demoNoFog = false;
    static int demoTarget = 0;
    static int demoTrigger = 1;
    static float demoFov = 180.0f;
    static float demoFireControl = 0.25f;

    if (!didTryInitialLoad) {
        if (AetherXMenuLoadConfig(configPath, settings)) {
            std::snprintf(status, sizeof(status), "Loaded preview preferences.");
        }
        didTryInitialLoad = true;
    }

    const ImVec2 display = ImGui::GetIO().DisplaySize;
    // Leave a slim outer margin so the floating logo launcher remains visible.
    const float width = std::max(280.0f, display.x * 0.80f);
    const float height = std::max(260.0f, display.y * 0.78f);
    ImGui::SetNextWindowPos(ImVec2(display.x * 0.5f, display.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
        settings.compactSpacing ? ImVec2(6.0f, 5.0f) : ImVec2(8.0f, 7.0f));
    if (ImGui::Begin("RAMBO | ImGui Preview", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        ImGui::TextUnformatted("RAMBO");
        ImGui::SameLine();
        ImGui::TextDisabled("NATIVE CONTROL CENTER");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 72.0f);
        ImGui::TextDisabled("PREVIEW");
        ImGui::Separator();

        if (settings.showPerformancePanel && settings.showFps) {
            const ImGuiIO& io = ImGui::GetIO();
            ImGui::TextDisabled("UI FPS: %.0f", io.Framerate);
            if (settings.showFrameTime && io.Framerate > 0.0f) {
                ImGui::SameLine();
                ImGui::TextDisabled("Frame: %.2f ms", 1000.0f / io.Framerate);
            }
            ImGui::Separator();
        }

        if (ImGui::BeginTabBar("##RAMBOPreviewTabs")) {
            if (ImGui::BeginTabItem("Dashboard")) {
                ImGui::Spacing();
                ImGui::TextUnformatted("Menu preview");
                ImGui::TextWrapped("This screen previews the tab layout and stock Dear ImGui dark colors. Switches are local demo values only.");
                ImGui::Separator();
                DrawMetric("UI framework", "Dear ImGui");
                DrawMetric("Render backend", "OpenGL ES 3");
                DrawMetric("Palette", "Stock dark");
                ImGui::Spacing();
                ImGui::Checkbox("Demo aim control", &demoAim);
                ImGui::Checkbox("Demo automatic fire", &demoAutoFire);
                ImGui::Checkbox("Demo visual overlay", &demoEsp);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Combat")) {
                ImGui::Spacing();
                ImGui::Checkbox("Aim control (preview)", &demoAim);
                ImGui::Checkbox("Automatic fire (preview)", &demoAutoFire);
                static const char* targets[] = { "Distance", "Crosshair" };
                static const char* triggers[] = { "None", "Shooting", "Scoping", "Both", "Any" };
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::Combo("Target selection", &demoTarget, targets, IM_ARRAYSIZE(targets));
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::Combo("Activation", &demoTrigger, triggers, IM_ARRAYSIZE(triggers));
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::SliderFloat("Field of view", &demoFov, 0.0f, 500.0f, "%.0f");
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::SliderFloat("Fire control", &demoFireControl, 0.0f, 0.70f, "%.2f");
                ImGui::TextDisabled("Demo controls are not connected to game memory.");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Visuals")) {
                ImGui::Spacing();
                ImGui::Checkbox("Visual overlay (preview)", &demoEsp);
                ImGui::Separator();
                ImGui::Columns(2, "##previewVisuals", false);
                ImGui::Checkbox("Boxes", &demoBox);
                ImGui::Checkbox("Skeleton", &demoSkeleton);
                ImGui::NextColumn();
                ImGui::Checkbox("Names", &demoNames);
                ImGui::Checkbox("Loot boxes", &demoLoot);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Misc")) {
                ImGui::Spacing();
                ImGui::Checkbox("Parachute option (preview)", &demoParachute);
                ImGui::Checkbox("Unlimited ammo option (preview)", &demoAmmo);
                ImGui::Checkbox("No fog option (preview)", &demoNoFog);
                ImGui::TextDisabled("These are illustrative UI switches only.");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings")) {
                ImGui::Spacing();
                ImGui::Checkbox("Compact spacing", &settings.compactSpacing);
                ImGui::Checkbox("Show UI FPS", &settings.showFps);
                ImGui::Checkbox("Show frame time", &settings.showFrameTime);
                ImGui::Checkbox("Show performance header", &settings.showPerformancePanel);
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::SliderFloat("Graph display window", &settings.graphWindowSeconds,
                                   3.0f, 30.0f, "%.0f sec");
                ImGui::Separator();
                if (ImGui::Button("Save settings", ImVec2(130.0f, 0.0f))) {
                    std::snprintf(status, sizeof(status), "%s",
                        AetherXMenuSaveConfig(configPath, settings)
                            ? "Saved preview settings."
                            : "Save failed: check private app storage.");
                }
                ImGui::SameLine();
                if (ImGui::Button("Load settings", ImVec2(130.0f, 0.0f))) {
                    std::snprintf(status, sizeof(status), "%s",
                        AetherXMenuLoadConfig(configPath, settings)
                            ? "Loaded preview settings."
                            : "Load failed: no saved file.");
                }
                ImGui::TextWrapped("%s", status);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("About")) {
                ImGui::Spacing();
                ImGui::TextUnformatted("RAMBO Native UI Preview");
                ImGui::TextDisabled("Dear ImGui / OpenGL ES 3");
                ImGui::Separator();
                ImGui::TextWrapped("This preview is hosted by the Java Activity and loads libaetherx_ui.so. It is separated from the game-process hook library.");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}
