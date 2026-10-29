# Build APK RAMBO ImGui Preview

Project ini dikonfigurasi untuk menghasilkan APK release arm64-v8a yang lebih ringan. APK berisi preview ImGui mandiri; ia tidak menyertakan game GTA SA atau SDK hook game yang tidak ada di source ZIP.

## Build lewat GitHub Actions
1. Upload seluruh isi folder project ini ke root repository GitHub.
2. Pastikan workflow ada di `.github/workflows/build.yml`.
3. Buka tab **Actions** → **Build RAMBO ImGui APK** → **Run workflow**.
4. Setelah berhasil, unduh artifact `RAMBO-ImGui-Preview-arm64-release`; di dalamnya ada `app-release.apk`.

Pengaturan ukuran: hanya ABI `arm64-v8a`, R8/ProGuard aktif, resource shrinking aktif, release tanpa debug symbols Java. Build belum dijalankan di lingkungan ini karena Android SDK/Gradle tidak tersedia di runner lokal; artifact APK harus dihasilkan oleh GitHub Actions.
