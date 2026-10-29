// Runtime isolation build: deliberately does not install hooks or patches into
// libanogs.so / anti-cheat components. Keep this file minimal while diagnosing
// the startup black-screen crash. RAMBO.cpp has its own native constructor.
#include <jni.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    if (vm == nullptr) return JNI_ERR;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *, void *) {}
