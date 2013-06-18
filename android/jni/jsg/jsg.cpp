#include "jsg.h"
#include "log.h"
#include "label.h"
#include "media_player.h"
#include "prefs.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>

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

JNIEnv* JSG::env;

ANativeWindow* window;

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

static void loadAssetFromAssetsDirectory(JNIEnv* env, const char* fileName, unsigned char** bytes, int* len)
{
  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/JSG");
    mid   = env->GetStaticMethodID(klass, "loadAsset", "(Ljava/lang/String;)[B");
  }

  // Will not be released because it's just a wrapper
  // Releasing will cause crash
  jstring jFileName = env->NewStringUTF(fileName);

  jbyteArray jba = (jbyteArray) env->CallStaticObjectMethod(klass, mid, jFileName);
  if (jba == NULL) {
    LOGE("Could not load asset: %s", fileName);
    *bytes = NULL;
    *len   = 0;
    return;
  }

  jbyte* cba = env->GetByteArrayElements(jba, NULL);
  if (cba == NULL) {
    LOGE("Could not load asset: %s", fileName);
    *bytes = NULL;
    *len   = 0;
    return;
  }

  *len   = env->GetArrayLength(jba);
  *bytes = (unsigned char*) malloc(*len);
  memcpy(*bytes, cba, *len);
  env->ReleaseByteArrayElements(jba, cba, 0);

  env->DeleteLocalRef(jFileName);
  env->DeleteLocalRef(jba);
}

#ifdef JSG_PROD
#include "jsg_assets_2c.h"
#endif

void JSG::loadAsset(const char* fileName, unsigned char** bytes, int* len)
{
#ifndef JSG_PROD
  loadAssetFromAssetsDirectory(env, fileName, bytes, len);
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

// Run on the main thread
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
  vm->GetEnv((void**) &JSG::env, JNI_VERSION_1_4);
  return JNI_VERSION_1_4;
}

// Run on the game thread
JNIEXPORT void JNICALL Java_js_g_JSG_nativeInit(JNIEnv* env, jclass klass, jint stageWidth, jint stageHeight, jstring mainScript)
{
  JSG::env = env;

  jsInitializeNative();
  jsLoadDefaults();

  const char *cmainScript = env->GetStringUTFChars(mainScript, NULL);
  char js[1024];
  sprintf(js, "jsg.load('%s');  jsg.fireReady(%d, %d)", cmainScript, stageWidth, stageHeight);
  node::Run(js);
  env->ReleaseStringUTFChars(mainScript, cmainScript);

  // Must be after jsg.fireReady because jsg.canvas is created at jsg.fireReady
  jsCacheCanvasAndOnFrame();
}

JNIEXPORT void JNICALL Java_js_g_JSG_runJS(JNIEnv* env, jclass klass, jstring js)
{
  const char *cjs = env->GetStringUTFChars(js, NULL);
  //LOGI("Run JS: %s", cjs);

  node::Run(cjs);
  env->ReleaseStringUTFChars(js, cjs);
}

JNIEXPORT void JNICALL Java_js_g_Stage_nativeSetSurface(JNIEnv* env, jclass klass, jobject surface)
{
  window = ANativeWindow_fromSurface(env, surface);
  ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
}

JNIEXPORT void JNICALL Java_js_g_Stage_nativeUnsetSurface(JNIEnv* env, jclass klass)
{
  ANativeWindow_release(window);
  window = NULL;
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
  unsigned char* dst = argb;
  for (int y = 0; y < height; y++) {
    uint32_t *row = (uint32_t *)(argb + (width * 4) * y);
    for (int x = 0; x < width; x++) {
      int bx = x * 4;
      uint32_t *pixel = row + x;
      uint8_t a = pixel[3];
      uint8_t r = pixel[2];
      uint8_t g = pixel[1];
      uint8_t b = pixel[0];
      pixel[0] = r;
      pixel[1] = g;
      pixel[2] = b;
      pixel[3] = a;
    }
  }
}

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
      argb2rgba(argb, width, height);
      memcpy(buffer.bits, argb,  width * height * 4);
      ANativeWindow_unlockAndPost(window);
    }
  }
}

}
