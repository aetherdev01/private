#include <cstring>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "KittyMemory/MemoryPatch.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Macros.h"
#include "Tools.h"
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#define _BYTE  uint8_t
#define _WORD  uint16_t
#define _DWORD uint32_t
#define _QWORD uint64_t

#define targetLibName OBFUSCATE("libUE4.so")
#define targetLibName OBFUSCATE("libanogs.so")
#define anogs OBFUSCATE("libanogs.so")

int (*sub_1DE3C4)(int a1, const char *a2);
int hsub_1DE3C4 (int a1, const char *a2) {
 if (strstr(a2, ("egl")) ||
       strstr(a2, ("unlocked")) ||
        strstr(a2, ("ro.")) ||
       strstr(a2, ("sys.oem_unlock_allowed")))
    {
        return 0LL;
    }
    return sub_1DE3C4(a1, a2);
}

int (*sub_1ECF74)(int a1, int *a2);
int hsub_1ECF74 (int a1, int *a2) {
while (true) {
     sleep (1000000); 
   }
   return sub_1ECF74(a1, a2);
}

int (*osub_144010)(_QWORD *a1, char *s);
int hsub_144010 (_QWORD *a1, char *s)
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_144010(a1,s);
}

int (*osub_12765C)(int a1);
int hsub_12765C (int a1)
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_12765C(a1);
}

int (*osub_127738)(int a1);
int hsub_127738 (int a1)
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_127738(a1);
}

int (*osub_3A61B8)(int a1, int a2, int a3);
int hsub_3A61B8 (int a1, int a2, int a3)
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_3A61B8(a1,a2,a3);
}

int (*sub_12B3DC)(const char *a1, unsigned int a2);
int osub_12B3DC(const char *a1, unsigned int a2)
 {    
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_12B3DC(a1,a2);
}

int (*osub_1EDAC8)(int a1, unsigned int a2, int a3);
int hsub_1EDAC8 (int a1, unsigned int a2, int a3) 
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_1EDAC8(a1,a2,a3);
}

int (*osub_381340)(_BYTE *a1, char a2, const char *a3, ...);
int hsub_381340(_BYTE *a1, char a2, const char *a3, ...) 
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_381340(a1,a2,a3);
}

int (*osub_27F8C0)(int a1, unsigned int a2, int a3);
int hsub_27F8C0(int a1, unsigned int a2, int a3) 
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_27F8C0(a1,a2,a3);
}

int (*osub_114154)(unsigned int a1, _BYTE *a2, int a3);
int hsub_114154 (unsigned int a1, _BYTE *a2, int a3) 
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_114154(a1,a2,a3);
}

int (*osub_33EF04)(int a1, int a2);
int hsub_33EF04 (int a1, int a2) 
{
std::this_thread::sleep_for(std::chrono::hours::max());
    return osub_33EF04(a1,a2);
}


void * hack_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded("libanogs.so"));
Tools::Hook((void *)(anogs + 
0x1ECF74), (void *) hsub_1ECF74, (void **) &sub_1ECF74);
Tools::Hook((void *)(anogs + 
0x1DE3C4), (void *) hsub_1DE3C4, (void **) &sub_1DE3C4);
Tools::Hook((void *)(anogs + 0x144010), (void *) hsub_144010, (void **) &osub_144010);
//Tools::Hook((void *)(anogs + 0x1AD3E4), (void *) hsub_1AD3E4, (void **) &osub_1AD3E4);
//Tools::Hook((void *)(anogs + 0x143994), (void *) hsub_1AD3E4, (void **) &osub_1AD3E4);
//Tools::Hook((void *)(anogs + 0x1B45CC), (void *) hsub_1AD3E4, (void **) &osub_1AD3E4);
Tools::Hook((void *)(anogs + 0x127738), (void *) hsub_127738, (void **) &osub_127738);
Tools::Hook((void *)(anogs + 0x12765C), (void *) hsub_12765C, (void **) &osub_12765C);
Tools::Hook((void *)(anogs + 0x12788C), (void *) hsub_12765C, (void **) &osub_12765C);
Tools::Hook((void *)(anogs + 0x127908), (void *) hsub_12765C, (void **) &osub_12765C);
Tools::Hook((void *)(anogs + 0x136770), (void *) hsub_12765C, (void **) &osub_12765C);
Tools::Hook((void *)(anogs + 0x3A61B8), (void *) hsub_3A61B8, (void **) &osub_3A61B8);
PATCH_LIB("libanogs.so", "0x34A5D0", "/x00/x20/x70/x47");
PATCH_LIB("libanogs.so","0xC839C","/x00/x20/x70/x47"); //Lobby Crash fix
PATCH_LIB("libanogs.so","0x12765C","/x00/x20/x70/x47"); //10 year fix
PATCH_LIB("libanogs.so","0xc839c ","/x00/x20/x70/x47");//-- Lobby Crash Fix
PATCH_LIB("libanogs.so","0x127738","/x00/x20/x70/x47");
PATCH_LIB("libanogs.so","0x12788c","/x00/x20/x70/x47");
PATCH_LIB("libanogs.so","0x127908","/x00/x20/x70/x47");
PATCH_LIB("libanogs.so","0x136770","/x00/x20/x70/x47");
PATCH_LIB("libanogs.so" , "0x133210","/x00/x20/x70/x47"); 
PATCH_LIB("libanogs.so" , "0x205D1C","/x00/x20/x70/x47"); 
PATCH_LIB("libanogs.so" , "0x1EAA50","/x00/x20/x70/x47"); 
//Tools::Hook((void *)(anogs + 0x1EAA50), (void *) hsub_1B45CC, (void **) &osub_1B45CC);
Tools::Hook((void *)(anogs + 0x1EDAC8), (void *) hsub_1EDAC8, (void **)&osub_1EDAC8);
Tools::Hook((void *)(anogs + 0x381340), (void *) hsub_381340, (void **) &osub_381340);
Tools::Hook((void *)(anogs + 0x27F8C0), (void *) hsub_27F8C0, (void **) &osub_27F8C0);
Tools::Hook((void *)(anogs + 0x114154), (void *) hsub_114154, (void **) &osub_114154);
Tools::Hook((void *)(anogs + 0x33EF04), (void *) hsub_33EF04, (void **) &osub_33EF04);

    return NULL;
}

void * zyroji_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded("libgcloud.so"));
    
//PATCH_LIB("libgcloud.so" , "0x261220","\x00\x00\xA0\xE3\x1E\xFF\x2F\xE1"); 

return NULL;
}

void * saurabh_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded("libanort.so"));
    
//HOOK_LIB("libanort.so", "0x12AD8C", osub_12B3DC, sub_12B3DC); // 27 x64

return NULL;
}

void *zyro_thread(void *){
    LOGI(OBFUSCATE("pthread created"));
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);


  return nullptr;
}
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    pthread_t libue4;
    pthread_create(&libue4, nullptr, zyro_thread, nullptr);
    pthread_t libanogs;
    pthread_create(&libanogs, nullptr, hack_thread, nullptr);
    pthread_t libgcloud;
    pthread_create(&libgcloud, nullptr, zyroji_thread, nullptr);
    pthread_t libanort;
    pthread_create(&libanort, nullptr, saurabh_thread, nullptr);
    
    return JNI_VERSION_1_6;
}
JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved) {
}

