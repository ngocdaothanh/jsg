LOCAL_PATH := $(call my-dir)

include jni/png.mk
include jni/pixman.mk
include $(CLEAR_VARS)

LIBCAIRO_SRC = \
	       cairo/src/cairo-analysis-surface.c            \
	       cairo/src/cairo-arc.c                         \
	       cairo/src/cairo-array.c                       \
	       cairo/src/cairo-atomic.c                      \
	       cairo/src/cairo-base64-stream.c               \
	       cairo/src/cairo-bentley-ottmann.c             \
	       cairo/src/cairo-bentley-ottmann-rectangular.c \
	       cairo/src/cairo-bentley-ottmann-rectilinear.c \
	       cairo/src/cairo-boxes.c                       \
	       cairo/src/cairo-boxes-intersect.c             \
	       cairo/src/cairo.c                             \
	       cairo/src/cairo-cache.c                       \
	       cairo/src/cairo-clip.c                        \
	       cairo/src/cairo-clip-polygon.c                \
	       cairo/src/cairo-clip-region.c                 \
	       cairo/src/cairo-clip-boxes.c                  \
	       cairo/src/cairo-clip-surface.c                \
	       cairo/src/cairo-color.c                       \
	       cairo/src/cairo-composite-rectangles.c        \
	       cairo/src/cairo-compositor.c                  \
	       cairo/src/cairo-contour.c                     \
	       cairo/src/cairo-damage.c                      \
	       cairo/src/cairo-debug.c                       \
	       cairo/src/cairo-default-context.c             \
	       cairo/src/cairo-deflate-stream.c              \
	       cairo/src/cairo-device.c                      \
	       cairo/src/cairo-error.c                       \
	       cairo/src/cairo-fixed.c                       \
	       cairo/src/cairo-font-face.c                   \
	       cairo/src/cairo-font-face-twin.c              \
	       cairo/src/cairo-font-face-twin-data.c         \
	       cairo/src/cairo-font-options.c                \
	       cairo/src/cairo-freelist.c                    \
         cairo/src/cairo-ft-font.c                     \
	       cairo/src/cairo-mesh-pattern-rasterizer.c     \
	       cairo/src/cairo-gstate.c                      \
	       cairo/src/cairo-hash.c                        \
	       cairo/src/cairo-hull.c                        \
	       cairo/src/cairo-image-compositor.c            \
	       cairo/src/cairo-image-info.c                  \
	       cairo/src/cairo-image-source.c                \
	       cairo/src/cairo-image-surface.c               \
	       cairo/src/cairo-lzw.c                         \
	       cairo/src/cairo-matrix.c                      \
	       cairo/src/cairo-mask-compositor.c             \
	       cairo/src/cairo-misc.c                        \
	       cairo/src/cairo-mono-scan-converter.c         \
	       cairo/src/cairo-mutex.c                       \
	       cairo/src/cairo-no-compositor.c               \
	       cairo/src/cairo-observer.c                    \
	       cairo/src/cairo-output-stream.c               \
	       cairo/src/cairo-paginated-surface.c           \
	       cairo/src/cairo-path-bounds.c                 \
	       cairo/src/cairo-path.c                        \
	       cairo/src/cairo-path-fill.c                   \
	       cairo/src/cairo-path-fixed.c                  \
	       cairo/src/cairo-path-in-fill.c                \
	       cairo/src/cairo-path-stroke.c                 \
	       cairo/src/cairo-path-stroke-boxes.c           \
	       cairo/src/cairo-path-stroke-polygon.c         \
	       cairo/src/cairo-pattern.c                     \
	       cairo/src/cairo-pen.c                         \
	       cairo/src/cairo-png.c                         \
	       cairo/src/cairo-polygon.c                     \
	       cairo/src/cairo-polygon-intersect.c           \
	       cairo/src/cairo-polygon-reduce.c              \
	       cairo/src/cairo-raster-source-pattern.c       \
	       cairo/src/cairo-recording-surface.c           \
	       cairo/src/cairo-rectangle.c                   \
	       cairo/src/cairo-rectangular-scan-converter.c  \
	       cairo/src/cairo-region.c                      \
	       cairo/src/cairo-scaled-font.c                 \
	       cairo/src/cairo-scaled-font-subsets.c         \
	       cairo/src/cairo-shape-mask-compositor.c       \
	       cairo/src/cairo-slope.c                       \
	       cairo/src/cairo-spans.c                       \
	       cairo/src/cairo-spans-compositor.c            \
	       cairo/src/cairo-spline.c                      \
	       cairo/src/cairo-stroke-style.c                \
	       cairo/src/cairo-stroke-dash.c                 \
	       cairo/src/cairo-surface.c                     \
	       cairo/src/cairo-surface-clipper.c             \
	       cairo/src/cairo-surface-fallback.c            \
         cairo/src/cairo-surface-offset.c              \
	       cairo/src/cairo-surface-snapshot.c            \
         cairo/src/cairo-surface-subsurface.c          \
	       cairo/src/cairo-surface-wrapper.c             \
	       cairo/src/cairo-tee-surface.c                 \
	       cairo/src/cairo-tor-scan-converter.c          \
         cairo/src/cairo-tor22-scan-converter.c        \
	       cairo/src/cairo-toy-font-face.c               \
	       cairo/src/cairo-traps.c                       \
	       cairo/src/cairo-traps-compositor.c            \
	       cairo/src/cairo-unicode.c                     \
	       cairo/src/cairo-user-font.c                   \
	       cairo/src/cairo-version.c                     \
	       cairo/src/cairo-wideint.c

LIBCAIRO_CFLAGS:=                                                   \
    -DPACKAGE_VERSION="\"android-cairo\""                           \
    -DPACKAGE_BUGREPORT="\"http://github.com/anoek/android-cairo\"" \
    -DCAIRO_NO_MUTEX=1                                              \
    -DHAVE_STDINT_H                                                 \
    -DCAIRO_HAS_PNG_FUNCTIONS=1                                     \
    -DCAIRO_HAS_FT_FONT=1                                           \
    -Ijni/png -Ijni/cairo -Ijni/cairo/src -Ijni/cairo-extra -Ijni/freetype2/include -include "cairo-version.h"

LOCAL_MODULE    := libcairo
LOCAL_CFLAGS    := $(LIBPIXMAN_CFLAGS) $(LIBCAIRO_CFLAGS)
LOCAL_LDFLAGS   := -lz
LOCAL_SRC_FILES := $(LIBPNG_SRC) $(LIBPIXMAN_SRC) $(LIBCAIRO_SRC)
LOCAL_STATIC_LIBRARIES := cpufeatures

include $(BUILD_STATIC_LIBRARY)
