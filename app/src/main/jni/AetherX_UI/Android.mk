LOCAL_PATH := $(call my-dir)
JNI_ROOT := $(abspath $(LOCAL_PATH)/..)
IMGUI_ROOT := $(JNI_ROOT)/imgui

include $(CLEAR_VARS)
LOCAL_MODULE := aetherx_ui
LOCAL_CPPFLAGS := -std=c++17 -DIMGUI_IMPL_OPENGL_ES3 -fvisibility=hidden
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(JNI_ROOT) $(IMGUI_ROOT) $(IMGUI_ROOT)/backends
LOCAL_SRC_FILES := \
    aether.cpp \
    ../imgui/imgui.cpp \
    ../imgui/imgui_draw.cpp \
    ../imgui/imgui_tables.cpp \
    ../imgui/imgui_widgets.cpp \
    ../imgui/backends/imgui_impl_android.cpp \
    ../imgui/backends/imgui_impl_opengl3.cpp
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3
include $(BUILD_SHARED_LIBRARY)
