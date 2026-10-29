// RAMBO loading-stability diagnostic stub.
// Intentionally does not install hooks or patch game/anti-cheat libraries.
// This isolates crashes caused by startup-time native hooks.
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "RAMBO-Stability"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    (void)vm;
    (void)reserved;
    LOGI("JNI_OnLoad reached; startup hooks are disabled for diagnosis");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    (void)vm;
    (void)reserved;
    LOGI("JNI_OnUnload");
}
