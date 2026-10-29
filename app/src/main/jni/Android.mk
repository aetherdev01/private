# The app's button loads this self-contained preview library.
LOCAL_PATH := $(call my-dir)
RAMBO_JNI_ROOT := $(LOCAL_PATH)
include $(RAMBO_JNI_ROOT)/AetherX_UI/Android.mk

# Build the original game-process target only in a complete source tree. The
# provided ZIP is missing these dependencies, so the standalone preview still
# builds instead of failing because unrelated game SDK assets are absent.
RAMBO_REQUIRED_FILES := \
    $(RAMBO_JNI_ROOT)/Dobby/$(TARGET_ARCH_ABI)/libdobby.a \
    $(RAMBO_JNI_ROOT)/curl/curl-android-$(TARGET_ARCH_ABI)/lib/libcurl.a \
    $(RAMBO_JNI_ROOT)/curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libssl.a \
    $(RAMBO_JNI_ROOT)/curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libcrypto.a \
    $(RAMBO_JNI_ROOT)/FTools/ImguiPP.cpp \
    $(RAMBO_JNI_ROOT)/Helper/ElfImg.cpp \
    $(RAMBO_JNI_ROOT)/SDK/LITE_Basic.cpp
ifneq ($(words $(wildcard $(RAMBO_REQUIRED_FILES))),$(words $(RAMBO_REQUIRED_FILES)))
    $(warning RAMBOSRC skipped: game SDK/FTools/Helper/Dobby/curl dependencies are missing; building ImGui preview only)
else
    include $(RAMBO_JNI_ROOT)/Android.mk.rambo
endif
