LOCAL_PATH := $(call my-dir)

include jni/freetype2.mk
include jni/cairo.mk
include $(CLEAR_VARS)

NODE_SRC = \
         node/node.cpp \
         node/node_buffer.cpp

CANVAS_SRC = \
	       canvas/Canvas.cpp \
	       canvas/CanvasGradient.cpp \
	       canvas/CanvasPattern.cpp \
	       canvas/CanvasRenderingContext2d.cpp \
	       canvas/color.cpp \
	       canvas/Image.cpp \
	       canvas/ImageData.cpp \
	       canvas/PixelArray.cpp \
	       canvas/FontFace.cpp

JSG_SRC = \
         jsg/label.cpp \
         jsg/matrix.cpp \
         jsg/log.cpp \
         jsg/jsg.cpp \
         jsg/media_player.cpp \
         jsg/prefs.cpp

JSG_CFLAGS := -Ijni/v8/include -Ijni/node -Ijni/canvas

LOCAL_MODULE    := jsg
LOCAL_CFLAGS    := $(LIBPIXMAN_CFLAGS) $(LIBCAIRO_CFLAGS) $(JSG_CFLAGS)

#ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lz -Ljni/v8/armeabi-v7a -lv8 -lGLESv2
#else
#LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lz -Ljni/v8/armeabi     -lv8 -lGLESv2
#endif

LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lz -Ljni/v8 -lv8 -lGLESv2

LOCAL_SRC_FILES := $(NODE_SRC) $(CANVAS_SRC) $(JSG_SRC)
LOCAL_STATIC_LIBRARIES := libpng libpixman libcairo libfreetype2

include $(BUILD_SHARED_LIBRARY)
