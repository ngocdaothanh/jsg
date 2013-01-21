LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LIBPNG_SRC= \
         libpng/png.c \
         libpng/pngerror.c \
         libpng/pngget.c \
         libpng/pngmem.c \
         libpng/pngpread.c \
         libpng/pngread.c \
         libpng/pngrio.c \
         libpng/pngrtran.c \
         libpng/pngrutil.c \
         libpng/pngset.c \
         libpng/pngtrans.c \
         libpng/pngwio.c \
         libpng/pngwrite.c \
         libpng/pngwtran.c \
         libpng/pngwutil.c

LOCAL_MODULE    := libpng
LOCAL_LDLIBS    := -lz
LOCAL_SRC_FILES := $(LIBPNG_SRC)

include $(BUILD_STATIC_LIBRARY)
