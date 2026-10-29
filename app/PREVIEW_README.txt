Refactor menu ImGui RAMBO
========================

Perubahan:
- MainActivity memiliki tombol untuk memuat libaetherx_ui.so dan membuka preview ImGui interaktif.
- floating_logo_data.h dipakai sebagai tekstur tombol logo mengambang; ketukan membuka/menutup menu bertab.
- Preview memakai warna default Dear ImGui (dark palette), menu bertab, dan tombol logo yang sama untuk membuka/menutup menu.
- Bagian UI dalam RAMBO.cpp dirapikan menjadi tab Dashboard, Combat, Visuals, Misc, Settings, dan About. Menu tertutup secara default dan dibuka/ditutup lewat tombol logo mengambang; logika fitur/game hook yang sudah ada tidak ditulis ulang.
- File UI duplikat XD.cpp, AetherX_UI/aether.cpp, dan AetherX_UI/aether.h dihapus.
- Menu preview berjalan terpisah dari library hook game karena RAMBO.cpp mengandalkan EGL swap-chain dan proses game.

Catatan build:
ZIP sumber awal tidak menyertakan beberapa dependensi yang dirujuk Android.mk lama, termasuk Dobby, curl, FTools, Helper, dan SDK source files. Karena itu Android.mk membangun library preview yang mandiri dan melewati target RAMBOSRC bila dependensinya tidak tersedia. Target opsional tetap didefinisikan di Android.mk.rambo; ketika semua dependensi lengkap, target itu ikut dibangun. Tombol Java memuat libaetherx_ui.so untuk menampilkan preview, bukan untuk menjalankan hook game. RAMBO.cpp sudah memiliki layout UI baru, tetapi target hook game tetap membutuhkan dependensi proyek lengkap untuk dapat dibangun.

Interaksi tombol logo:
- Tombol logo mengambang dirender dari floating_logo_data.h sebagai tekstur RGBA.
- Ketuk logo untuk membuka menu bertab, ketuk kembali untuk menutupnya.
- Jika tekstur gagal dimuat, tombol teks RAMBO/Close menjadi fallback.
