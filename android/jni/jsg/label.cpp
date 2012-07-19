#include "label.h"
#include "jsg.h"

#include <stdlib.h>
#include <android/bitmap.h>
#include <canvas/Canvas.h>

using namespace v8;

void Label::InitializeNative(Handle<ObjectTemplate> global)
{
  global->Set(String::New("jsgDrawLabel"), FunctionTemplate::New(Label::DrawLabel));
}

Handle<Value> Label::DrawLabel(const Arguments& args)
{
  HandleScope scope;

  Local<Object> obj = args[0]->ToObject();
  if (!Canvas::constructor->HasInstance(obj))
    return ThrowException(Exception::TypeError(String::New("Canvas expected")));

  Canvas* canvas = ObjectWrap::Unwrap<Canvas>(obj);
  String::Utf8Value vfontName (args[1]);
  int               fontSize = args[2]->Int32Value();
  String::Utf8Value vtext     (args[3]);
  String::Utf8Value vtextColor(args[4]);
  int               alignx   = args[5]->Int32Value();
  int               aligny   = args[6]->Int32Value();

  int width  = canvas->width;
  int height = canvas->height;

  cairo_surface_t* surface = canvas->surface();
  cairo_surface_flush(surface);
  unsigned char* argb   = cairo_image_surface_get_data(surface);
  //int            stride = cairo_image_surface_get_stride(surface);

  draw(argb, width, height, *vfontName, fontSize, *vtext, *vtextColor, alignx, aligny);
  cairo_surface_mark_dirty(surface);

  return Undefined();
}

void Label::draw(
    unsigned char* argbTarget, int width, int height,
    const char* fontName, int fontSize,
    const char* text, const char* textColor, int alignx, int aligny)
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/Label");
    mid   = env->GetStaticMethodID(klass, "createTextBitmap", "(IILjava/lang/String;ILjava/lang/String;Ljava/lang/String;II)Landroid/graphics/Bitmap;");
  }

  jstring jfontName  = env->NewStringUTF(fontName);
  jstring jtext      = env->NewStringUTF(text);
  jstring jtextColor = env->NewStringUTF(textColor);

  jobject bitmap = env->CallStaticObjectMethod(klass, mid, width, height, jfontName, fontSize, jtext, jtextColor, alignx, aligny);

  void* addrPtr;
  AndroidBitmap_lockPixels(env, bitmap, &addrPtr);
  memcpy(argbTarget, addrPtr, width * height * 4);
  AndroidBitmap_unlockPixels(env, bitmap);

  env->DeleteLocalRef(jfontName);
  env->DeleteLocalRef(jtext);
  env->DeleteLocalRef(jtextColor);
}
