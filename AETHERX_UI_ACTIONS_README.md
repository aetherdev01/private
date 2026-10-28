# AetherX UI + GitHub Actions overlay

Copy these files into the root of the existing repository (the directory containing `app/` and `build.gradle`).

The workflow must be under `.github/workflows/build.yml`—not `.github/actions/build.yml`, because GitHub discovers workflow definitions from `.github/workflows/`.

## Build behavior

- Builds `app/src/main/jni/AetherX_UI/aether.cpp` and the Dear ImGui core/Android/OpenGL3 backend into `libaetherx_ui.so` for `arm64-v8a`.
- The workflow intentionally uses this dedicated `AetherX_UI/Android.mk`. It does **not** compile `RAMBO.cpp`, hooks, gameplay memory patches, aim assistance, automated player tracking, or anti-cheat bypass code.
- The workflow uploads only the `.so` as an artifact. It does not upload the project source as an artifact.
- This creates a UI library; it does not by itself initialize ImGui or inject/connect the menu into a game renderer.

## Apply from Termux

From the existing project root, extract this overlay ZIP, then run:

```bash
git add .github/workflows/build.yml app/src/main/jni/AetherX_UI/aether.cpp app/src/main/jni/AetherX_UI/aether.h app/src/main/jni/AetherX_UI/Android.mk app/src/main/jni/AetherX_UI/Application.mk AETHERX_UI_ACTIONS_README.md
git commit -m "Add UI-only GitHub Actions build"
git push
```

Run it manually in GitHub: **Actions → Build AetherX UI Native Library → Run workflow**. When successful, download the `aetherx-ui-so-arm64-v8a` artifact.
