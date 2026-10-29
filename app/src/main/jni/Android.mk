# Native RAMBO module only. No Java preview or parallel UI implementation.
LOCAL_PATH := $(call my-dir)
RAMBO_JNI_ROOT := $(LOCAL_PATH)
include $(RAMBO_JNI_ROOT)/Android.mk.rambo
