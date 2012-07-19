LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LIBPNG_SRC= \
         png/png.c \
         png/pngerror.c \
         png/pngget.c \
         png/pngmem.c \
         png/pngpread.c \
         png/pngread.c \
         png/pngrio.c \
         png/pngrtran.c \
         png/pngrutil.c \
         png/pngset.c \
         png/pngtrans.c \
         png/pngwio.c \
         png/pngwrite.c \
         png/pngwtran.c \
         png/pngwutil.c

LOCAL_MODULE    := libpng
LOCAL_LDLIBS    := -lz
LOCAL_SRC_FILES := $(LIBPNG_SRC)

include $(BUILD_STATIC_LIBRARY)
