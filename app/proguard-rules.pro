# JNI entry points are called by name from native code; keep these methods intact.
-keep class com.alfas.xd.MainActivity { *; }
-keepclasseswithmembernames class * { native <methods>; }
-dontwarn **
