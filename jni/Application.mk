APP_ABI := all
APP_PLATFORM := android-8

NDK_TOOLCHAIN_VERSION := clang
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti
APP_CPPFLAGS += -fexceptions
APP_BUILD_SCRIPT := Android.mk