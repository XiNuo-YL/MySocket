# use libc++_static #
APP_STL := stlport_static
APP_ABI := armeabi
#APP_ABI := armeabi arm64-v8a
#APP_ABI := armeabi-v7a
APP_PLATFORM := android-21
#APP_OPTIM := debug
APP_OPTIM := release
include $(call my-dir)
