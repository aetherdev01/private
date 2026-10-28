# AetherX UI component — combined source package

This folder contains the original uploaded project plus an AetherX-branded Dear ImGui UI component placed at:

`app/src/main/jni/AetherX_UI/AetherX_Menu_UI.cpp`
`app/src/main/jni/AetherX_UI/AetherX_Menu_UI.h`

## What was changed

- Added the AetherX UI source/header to the native project.
- Added `AetherX_UI/AetherX_Menu_UI.cpp` to `Android.mk` so it is part of the native build.
- Added the UI include directory to the native include paths.
- Reuses the project's existing Dear ImGui 1.85 WIP files; no duplicate ImGui core was added.

## Important limitation

The UI component is compiled as a separate UI module, but it has **not** been wired into the project's live render loop or connected to gameplay toggles. Its settings are UI-only (display FPS/frame time, compact spacing, display preferences, local config save/load). It does not implement or connect aim assistance, player tracking, ESP, memory patches, or anti-cheat bypass. Therefore this package is a unified source tree, not a finished APK/SO with a newly active menu.

## Build verification

The source layout and `Android.mk` edits were checked, but a full NDK build was not run in this environment. The existing project may have independent build issues or third-party requirements. Build with the Android NDK environment already used by the original project, then resolve any compiler errors before relying on the output.

## Dear ImGui

The UI uses the same Dear ImGui API version already bundled in this project (`1.85 WIP`, `IMGUI_VERSION_NUM 18416`). Review upstream licensing terms before redistributing third-party source.
