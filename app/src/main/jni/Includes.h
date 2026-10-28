#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/uio.h>

#include <fcntl.h>
#include <android/log.h>
#include <pthread.h>
#include <dirent.h>
#include <list>
#include <libgen.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include <codecvt>
#include <chrono>
#include <queue>

using namespace std::chrono_literals;

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES/glplatform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>

#include <sys/system_properties.h>
#include <imgui/enc.h>
#include <imgui/imgui.h>
#include <imgui/android_native_app_glue.h>
#include <imgui/backends/imgui_impl_android.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#define LOG_TAG "Chitoge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define HOOK(target, hook, original) Tools::Hook((void *)(target), (void *)(hook), (void **)(original))
#define XORS(string) []() { static auto result = SPra::Xor_String<sizeof(string)/sizeof(char)>(string); return result(); }()
