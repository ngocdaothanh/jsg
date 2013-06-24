#include "jsg.h"
#include "log.h"
#include "label.h"
#include "media_player.h"
#include "prefs.h"

#include <android/native_window.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <canvas/FontFace.h>
#include <canvas/Canvas.h>

using namespace v8;

//#define JSG_PROD

//------------------------------------------------------------------------------

// scope:  For avoiding error "Cannot create a handle without a HandleScope"
// global: Can't be initialized to ObjectTemplate::New() here due to threading problem
static HandleScope            scope;
static Handle<ObjectTemplate> global;
static Persistent<Context>    context;

static Persistent<Object>   jsgObject;
static Persistent<Object>   jsgCanvasObject;
static Persistent<Function> jsgOnFrameFun;

//------------------------------------------------------------------------------

// See:
//
// android-ndk/platforms/android-8/arch-arm/usr/include/jni.h
// http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
//
// http://stackoverflow.com/questions/8502269/how-do-i-keep-a-jobject-reference-alive-behind-jni-call
// http://java.sun.com/developer/onlineTraining/Programming/JDCBook/jniref.html#memory
// http://android-developers.blogspot.jp/2011/11/jni-local-reference-changes-in-ics.html

android_app* JSG::app;
bool         JSG::animating;
JNIEnv*      JSG::env;

EGLDisplay JSG::eglDisplay;
EGLContext JSG::eglContext;
EGLSurface JSG::eglSurface;

cairo_device_t*  JSG::cairoDevice;
cairo_surface_t* JSG::windowSurface;

//------------------------------------------------------------------------------

static void jsInitializeNative()
{
  HandleScope scope;

  global = ObjectTemplate::New();

  // Bootstrap JS environment

  Console::InitializeNative(global);
  Canvas::InitializeNative(global);
  Label::InitializeNative(global);
  MediaPlayer::InitializeNative(global);
  Prefs::InitializeNative(global);

  global->Set(String::New("jsgLoad"),                      FunctionTemplate::New(JSG::Load));
  global->Set(String::New("jsgLoadString"),                FunctionTemplate::New(JSG::LoadString));
  global->Set(String::New("jsgFontFace"),                  FunctionTemplate::New(JSG::FontFace));
  global->Set(String::New("jsgCallJava"),                  FunctionTemplate::New(JSG::CallJava));
  global->Set(String::New("jsgSaveCanvasToSystemGallery"), FunctionTemplate::New(JSG::SaveCanvasToSystemGallery));

  context = Context::New(NULL, global);
  context->Enter();
  global = scope.Close(global);
}

// http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
static bool endsWith(const char* full, const char* ending)
{
  int fullLen   = strlen(full);
  int endingLen = strlen(ending);
  if (fullLen < endingLen) return false;

  int startIndex = fullLen - endingLen;
  return (0 == strcmp(full + startIndex, ending));
}

static bool beginsWith(const char* full, const char* beginning)
{
  int fullLen      = strlen(full);
  int beginningLen = strlen(beginning);
  if (fullLen < beginningLen) return false;

  for (int i = 0; i < beginningLen; i++) {
    if (full[i] != beginning[i]) return false;
  }
  return true;
}

static void jsLoad(const char* jsFileName)
{
  HandleScope scope;

  unsigned char* bytes;
  int            len;
  JSG::loadAsset(jsFileName, &bytes, &len);
  if (bytes == NULL) return;

  node::Run(bytes, len, jsFileName);
  free(bytes);
}

static void jsLoadDefaults()
{
  Console::LoadScript();
  Canvas::LoadScript();

  jsLoad("scripts/jsg/platform/android/before.js");
  jsLoad("scripts/jsg/jsg.js");
  jsLoad("scripts/jsg/platform/android/after.js");
}

static void jsCacheCanvasAndOnFrame()
{
  HandleScope scope;

  // https://groups.google.com/forum/?fromgroups#!topic/v8-users/RqdmlC0QHgY
  jsgObject       = Persistent<Object>::New(Handle<Object>::Cast(context->Global()->Get(String::New("jsg"))));
  jsgCanvasObject = Persistent<Object>::New(Handle<Object>::Cast(jsgObject->Get(String::New("canvas"))));
  jsgOnFrameFun   = Persistent<Function>::New(Handle<Function>::Cast(jsgObject->Get(String::New("onFrame"))));
}

static void jsDestroy()
{
  context->Exit();
  context.Dispose();
}

//------------------------------------------------------------------------------

static void loadAssetFromAssetsDirectory(const char* fileName, unsigned char** bytes, int* len)
{
  AAssetManager* mgr = JSG::app->activity->assetManager;

  AAsset* asset = AAssetManager_open(mgr, fileName, AASSET_MODE_UNKNOWN);
  if (NULL == asset) {
    LOGE("Could not load asset: %s", fileName);
    *bytes = NULL;
    *len   = 0;
    return;
  }

  *len   = AAsset_getLength(asset);
  *bytes = (unsigned char*) malloc(*len);
  AAsset_read(asset, *bytes, *len);
  AAsset_close(asset);
}

#ifdef JSG_PROD
#include "jsg_assets_2c.h"
#endif

void JSG::loadAsset(const char* fileName, unsigned char** bytes, int* len)
{
#ifndef JSG_PROD
  loadAssetFromAssetsDirectory(fileName, bytes, len);
#else
  if (beginsWith(fileName, "scripts/")) {
    jsg_assets_2c_load(fileName, bytes, len);
    if (*bytes == NULL) LOGE("Could not load asset: %s", fileName);
  } else {
    loadAssetFromAssetsDirectory(env, fileName, bytes, len);
  }
#endif
}

Handle<Value> JSG::LoadString(const Arguments& args)
{
  HandleScope scope;

  String::Utf8Value fileName(args[0]);
  const char* cFileName = *fileName;

  unsigned char* bytes;
  int            len;
  JSG::loadAsset(cFileName, &bytes, &len);

  return scope.Close(String::New((char*) bytes, len));
}

Handle<Value> JSG::Load(const Arguments& args)
{
  String::Utf8Value fileName(args[0]);
  const char* cFileName = *fileName;
  jsLoad(cFileName);
  return Undefined();
}

Handle<Value> JSG::FontFace(const Arguments& args)
{
  String::Utf8Value fontFamily(args[0]);
  String::Utf8Value src(args[1]);
  const char* cFontFamily = *fontFamily;
  const char* cSrc        = *src;

  // Avoid memory leak because bytes below will not be freed!
  if (FontFace::get(cFontFamily)) return Undefined();

  unsigned char* bytes;
  int            len;
  JSG::loadAsset(cSrc, &bytes, &len);
  if (!FontFace::load(cFontFamily, bytes, len))
    LOGE("Could not load font: %s -> %s", cSrc, cFontFamily);

  // free(bytes) will make the font unusable!

  return Undefined();
}

Handle<Value> JSG::CallJava(const Arguments& args)
{
  HandleScope scope;

  String::Utf8Value vclassName(args[0]);
  String::Utf8Value vstaticMethodName(args[1]);

  char buffer[200];

  jclass klass = env->FindClass(*vclassName);
  if (!klass) {
    sprintf(buffer, "Java class %s not found", *vclassName);
    return ThrowException(String::New(buffer));
  }

  jmethodID mid;
  if (args[2]->IsString())
    mid = env->GetStaticMethodID(klass, *vstaticMethodName, "(Ljava/lang/String;)Ljava/lang/String;");
  else
    mid = env->GetStaticMethodID(klass, *vstaticMethodName, "()Ljava/lang/String;");

  if (!mid) {
    sprintf(buffer, "Suitable static method %s not found in Java class %s", *vstaticMethodName, *vclassName);
    return ThrowException(String::New(buffer));
  }

  env->DeleteLocalRef(klass);

  jstring jresult;
  if (args[2]->IsString()) {
    String::Utf8Value vmessage(args[2]);
    jstring           jmessage = env->NewStringUTF(*vmessage);

    jresult = (jstring) env->CallStaticObjectMethod(klass, mid, jmessage);
    env->DeleteLocalRef(jmessage);
  } else {
    jresult = (jstring) env->CallStaticObjectMethod(klass, mid);
  }

  if (jresult) {
    const char*  cresult = env->GetStringUTFChars(jresult, NULL);
    int          len     = env->GetStringUTFLength(jresult);
    Local<Value> vresult = (len == 0)? String::Empty() : String::New(cresult, len);

    env->ReleaseStringUTFChars(jresult, cresult);
    env->DeleteLocalRef(jresult);
    return scope.Close(vresult);
  } else {
    return Null();
  }
}

// args: canvas, title, description
Handle<Value> JSG::SaveCanvasToSystemGallery(const Arguments& args)
{
  // V8

  HandleScope scope;
  Local<Object> obj = args[0]->ToObject();

  if (!Canvas::constructor->HasInstance(obj))
    return ThrowException(Exception::TypeError(String::New("Canvas expected")));

  Canvas* canvas = ObjectWrap::Unwrap<Canvas>(obj);

  int width  = canvas->width;
  int height = canvas->height;

  cairo_surface_t* surface = canvas->surface();
  cairo_surface_flush(surface);
  unsigned char* argb   = cairo_image_surface_get_data(surface);
  int            stride = cairo_image_surface_get_stride(surface);

  String::Utf8Value vtitle      (args[1]);
  String::Utf8Value vdescription(args[2]);

  // Java

  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/JSG");
    mid   = env->GetStaticMethodID(klass, "saveBitmapToSystemGallery", "([IIIILjava/lang/String;Ljava/lang/String;)V");
  }

  // jarray will not be released because it's just a wrapper
  // Releasing will cause crash
  int       len    = width * height;
  jintArray jarray = env->NewIntArray(len);
  env->SetIntArrayRegion(jarray, 0, len, (jint*) argb);

  jstring jtitle       = env->NewStringUTF(*vtitle);
  jstring jdescription = env->NewStringUTF(*vdescription);

  env->CallStaticVoidMethod(klass, mid, jarray, stride / 4, width, height, jtitle, jdescription);

  env->DeleteLocalRef(jarray);
  env->DeleteLocalRef(jtitle);
  env->DeleteLocalRef(jdescription);

  return Undefined();
}

//------------------------------------------------------------------------------

extern "C" {

JNIEXPORT void JNICALL Java_js_g_JSG_runJS(JNIEnv* env, jclass klass, jstring js)
{
  const char *cjs = env->GetStringUTFChars(js, NULL);
  //LOGI("Run JS: %s", cjs);

  node::Run(cjs);
  env->ReleaseStringUTFChars(js, cjs);
}

static void drawFrame();
static void* gameLoop(void* dummy) {
  while (JSG::animating) {
    if (JSG::app->window != NULL) {
      if (eglMakeCurrent(JSG::eglDisplay, JSG::eglSurface, JSG::eglSurface, JSG::eglContext) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        exit(-1);
      }

      // Drawing is throttled to the screen update rate, so there
      // is no need to do timing here.
      drawFrame();
    }
  }

  return NULL;
}

// Run on the game thread
static void init(const char* mainScript)
{
  jsInitializeNative();
  jsLoadDefaults();

  ANativeWindow* window = JSG::app->window;
  ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
  int stageWidth  = ANativeWindow_getWidth(window);
  int stageHeight = ANativeWindow_getHeight(window);

  //---

  const EGLint attribs[] = {
    EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_BLUE_SIZE,       8,
    EGL_GREEN_SIZE,      8,
    EGL_RED_SIZE,        8,
    EGL_ALPHA_SIZE,      8,
    EGL_SAMPLES,         8,
    EGL_SAMPLE_BUFFERS,  1,
    EGL_NONE
  };

  const EGLint contextAttribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  JSG::eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(JSG::eglDisplay, 0, 0);

  EGLConfig config;
  EGLint numConfigs;
  eglChooseConfig(JSG::eglDisplay, attribs, &config, 1, &numConfigs);

  EGLint format;
  eglGetConfigAttrib(JSG::eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
  ANativeWindow_setBuffersGeometry(window, 0, 0, format);

  JSG::eglSurface = eglCreateWindowSurface(JSG::eglDisplay, config, window, NULL);
  JSG::eglContext = eglCreateContext(JSG::eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);

  if (eglMakeCurrent(JSG::eglDisplay, JSG::eglSurface, JSG::eglSurface, JSG::eglContext) == EGL_FALSE) {
    LOGI("Unable to eglMakeCurrent");
    exit(-1);
  }

  glViewport(0, 0, stageWidth, stageHeight);

  JSG::cairoDevice     = cairo_egl_device_create(JSG::eglDisplay, JSG::eglContext);
  //JSG::windowSurface = cairo_gl_surface_create_for_egl(JSG::cairoDevice, JSG::eglSurface, stageWidth, stageHeight);
  JSG::windowSurface = cairo_gl_surface_create_for_egl(JSG::cairoDevice, JSG::eglSurface, 512, 512);
  cairo_gl_device_set_thread_aware(JSG::cairoDevice, false);

  //---

  char js[1024];
  //sprintf(js, "jsg.load('%s');  jsg.fireReady(%d, %d)", mainScript, stageWidth, stageHeight);
  sprintf(js, "jsg.load('%s');  jsg.fireReady(%d, %d)", mainScript, 512, 512);
  node::Run(js);

  // Must be after jsg.fireReady because jsg.canvas is created at jsg.fireReady
  jsCacheCanvasAndOnFrame();

  pthread_t t;
  pthread_create(&t, NULL, &gameLoop, NULL);
}

/*
static void argb2rgba(unsigned char* argb, unsigned char* rgba, int width, int height)
{
  unsigned char* dst = rgba;
  for (int y = 0; y < height; y++) {
    uint32_t *row = (uint32_t *)(argb + (width * 4) * y);
    for (int x = 0; x < width; x++) {
      int bx = x * 4;
      uint32_t *pixel = row + x;
      uint8_t a = *pixel >> 24;
      uint8_t r = *pixel >> 16;
      uint8_t g = *pixel >> 8;
      uint8_t b = *pixel;
      dst[bx + 3] = a;
      dst[bx + 0] = r;
      dst[bx + 1] = g;
      dst[bx + 2] = b;
    }
    dst += width * 4;
  }
}
*/
static void argb2rgba(unsigned char* argb, int width, int height)
{
  for (int i = 0; i < width * height * 4; i++) {
    uint8_t a = argb[i + 3];
    uint8_t r = argb[i + 2];
    uint8_t g = argb[i + 1];
    uint8_t b = argb[i];

    argb[i    ] = r;
    argb[i + 1] = g;
    argb[i + 2] = b;
    argb[i + 3] = a;

    i += 4;
  }
}

/*
JNIEXPORT void JNICALL Java_js_g_Stage_nativeOnDrawFrame(JNIEnv* env, jclass klass,
    jintArray jtouchActions, jintArray jtouchXs, jintArray jtouchYs, jint numTouches)
{
  // This function is run every frame, thus must be optimized.
  // Avoid using node::Run because the script must be compiled every time.

  HandleScope scope;

  // Clear stage

  Canvas* canvas = ObjectWrap::Unwrap<Canvas>(jsgCanvasObject);
  int     width  = canvas->width;
  int     height = canvas->height;

  cairo_surface_t* surface = canvas->surface();
  //cairo_surface_flush(surface);
  unsigned char* argb = cairo_image_surface_get_data(surface);
  //memset(argb, 0, width * height * 4);
  //cairo_surface_mark_dirty(surface);

  // Fire events and draw to stage canvas

  if (numTouches > 0) {
    jint* touchActions = env->GetIntArrayElements(jtouchActions, NULL);
    jint* touchXs      = env->GetIntArrayElements(jtouchXs,      NULL);
    jint* touchYs      = env->GetIntArrayElements(jtouchYs,      NULL);

    Local<Array> vtouchActions = Array::New(numTouches);
    Local<Array> vtouchXs      = Array::New(numTouches);
    Local<Array> vtouchYs      = Array::New(numTouches);

    for (int i = 0; i < numTouches; i++) {
      int action = touchActions[i];
      int x      = touchXs     [i];
      int y      = touchYs     [i];
      vtouchActions->Set(Number::New(i), Number::New(action));
      vtouchXs     ->Set(Number::New(i), Number::New(x));
      vtouchYs     ->Set(Number::New(i), Number::New(y));
    }

    Handle<Value> args[] = { vtouchActions, vtouchXs, vtouchYs };
    jsgOnFrameFun->Call(jsgObject, 3, args);

    env->ReleaseIntArrayElements(jtouchActions, touchActions, 0);
    env->ReleaseIntArrayElements(jtouchXs,      touchXs,      0);
    env->ReleaseIntArrayElements(jtouchYs,      touchYs,      0);
  } else {
    jsgOnFrameFun->Call(jsgObject, 0, NULL);
  }

  // Copy stage canvas to surface

  if (window != NULL) {
    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(window, &buffer, NULL) == 0) {
      //argb2rgba(argb, width, height);
      memcpy(buffer.bits, argb,  width * height * 4);
      ANativeWindow_unlockAndPost(window);
    }
  }
}
*/

static void drawFrame()
{
  jsgOnFrameFun->Call(jsgObject, 0, NULL);

  Canvas* canvas = ObjectWrap::Unwrap<Canvas>(jsgCanvasObject);
  cairo_surface_t* surface = canvas->surface();

  cairo_t* windowCr = cairo_create(JSG::windowSurface);
  cairo_set_source_surface(windowCr, surface, 0, 0);
  cairo_paint(windowCr);
  cairo_destroy(windowCr);

  cairo_gl_surface_swapbuffers(JSG::windowSurface);
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      // The system has asked us to save our current state.  Do so.
      break;

    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      if (app->window != NULL) {
        init("scripts/js/main.coffee");
      }
      break;

    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      //engine_term_display(engine);
      break;

    case APP_CMD_GAINED_FOCUS:
      break;

    case APP_CMD_LOST_FOCUS:
      break;
  }
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(android_app* app, AInputEvent* event) {
  return 0;
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* app) {
  JSG::app = app;
  app->activity->vm->AttachCurrentThread(&JSG::env, NULL);

  // Make sure glue isn't stripped.
  app_dummy();

  app->onAppCmd = engine_handle_cmd;
  app->onInputEvent = engine_handle_input;

  if (app->savedState != NULL) {
    // We are starting with a previous saved state; restore from it.
  }

  JSG::animating = true;
  while (1) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    //while ((ident=ALooper_pollAll(JSG::animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
    while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0) {
      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) {
      }

      // Check if we are exiting.
      if (app->destroyRequested != 0) {
        JSG::animating = false;
        return;
      }
    }
  }
}

}
