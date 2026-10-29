#include <jni.h>
#include <algorithm>
#include <GLES3/gl3.h>
#include <chrono>
#include <cstdint>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "AetherX_Menu_UI.h"
#include "FloatingLogoTexture.h"

namespace {
bool g_initialized = false;
bool g_menuOpen = false;
AetherXMenuSettings g_settings;
std::string g_configPath;
int g_width = 1;
int g_height = 1;
std::chrono::steady_clock::time_point g_lastFrame;
bool g_hasLastFrame = false;

void DrawFloatingLogoButton() {
    const float size = std::min(56.0f, std::max(32.0f, static_cast<float>(g_height) * 0.09f));
    const float margin = 12.0f;
    const float x = g_width > static_cast<int>(size + margin)
        ? static_cast<float>(g_width) - size - margin : 4.0f;
    const float y = 4.0f;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("##RAMBO_PreviewFloatingLogo", nullptr, flags)) {
        if (RamboFloatingLogo::EnsureLoaded()) {
            if (ImGui::ImageButton(
                    (ImTextureID)(intptr_t)RamboFloatingLogo::gTexture,
                    ImVec2(size, size), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                    0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
                    ImVec4(1.0f, 1.0f, 1.0f, 1.0f))) {
                g_menuOpen = !g_menuOpen;
            }
        } else if (ImGui::Button(g_menuOpen ? "Close" : "RAMBO")) {
            g_menuOpen = !g_menuOpen;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(g_menuOpen ? "Close RAMBO menu" : "Open RAMBO menu");
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeSetConfigPath(JNIEnv* env, jclass, jstring path) {
    if (path == nullptr) {
        g_configPath.clear();
        return;
    }
    const char* chars = env->GetStringUTFChars(path, nullptr);
    if (chars != nullptr) {
        g_configPath.assign(chars);
        env->ReleaseStringUTFChars(path, chars);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeInit(JNIEnv*, jclass) {
    if (g_initialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    AetherXMenuApplyDefaultStyle();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.FontGlobalScale = 1.15f;
    g_initialized = ImGui_ImplOpenGL3_Init("#version 300 es");
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeResize(JNIEnv*, jclass, jint width, jint height) {
    g_width = width > 0 ? width : 1;
    g_height = height > 0 ? height : 1;
    if (g_initialized) {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)g_width, (float)g_height);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }
    glViewport(0, 0, g_width, g_height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeTouch(JNIEnv*, jclass, jfloat x, jfloat y, jboolean down) {
    if (!g_initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);
    io.MouseDown[0] = down == JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeRender(JNIEnv*, jclass) {
    if (!g_initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)g_width, (float)g_height);
    const auto now = std::chrono::steady_clock::now();
    if (g_hasLastFrame) {
        io.DeltaTime = std::chrono::duration<float>(now - g_lastFrame).count();
        if (io.DeltaTime <= 0.0f) io.DeltaTime = 1.0f / 60.0f;
    }
    g_lastFrame = now;
    g_hasLastFrame = true;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    AetherXMenuRender(&g_menuOpen, g_settings,
                      g_configPath.empty() ? nullptr : g_configPath.c_str());
    // Draw last so the floating logo remains clickable over the open menu.
    DrawFloatingLogoButton();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

extern "C" JNIEXPORT void JNICALL
Java_com_alfas_xd_MainActivity_nativeShutdown(JNIEnv*, jclass) {
    RamboFloatingLogo::ResetForNewContext();
    if (!g_initialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    g_initialized = false;
    g_hasLastFrame = false;
}
