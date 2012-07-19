#ifndef _JSG_H_
#define _JSG_H_

#include <jni.h>
#include <v8.h>

#include "log.h"

class JSG
{
 public:
  static JNIEnv* env;

 public:
  static inline JNIEnv* getJNIEnv()
  {
    return env;
  }

  /**
   * Converts JNI local ref to global ref to avoid error:
   * Failed adding to JNI local ref table (has 512 entries)
   *
   * http://groups.google.com/group/android-ndk/browse_thread/thread/4c5e6c6eaf06869b
   */
  static inline jobject refLocal2Global(jobject localRef)
  {
    if (!localRef) {
      LOGE("NULL passed to JSG::refLocal2Global");
      jclass exceptionClass = env->FindClass("java/lang/Exception");
      env->ThrowNew(exceptionClass, "NULL passed to JSG::refLocal2Global");
      env->DeleteLocalRef(exceptionClass);
      return NULL;
    } else {
      jobject ret = env->NewGlobalRef(localRef);
      env->DeleteLocalRef(localRef);
      return ret;
    }
  }

  static inline jclass findClassAndMakeGlobal(const char* className)
  {
    jclass localRef = env->FindClass(className);
    if (localRef) return (jclass) JSG::refLocal2Global(localRef);

    char msg[100];
    sprintf(msg, "Could not find Java class \"%s\"", className);
    LOGE(msg);
    jclass exceptionClass = env->FindClass("java/lang/Exception");
    env->ThrowNew(exceptionClass, msg);
    env->DeleteLocalRef(exceptionClass);
    return NULL;
  }

  static void loadAsset(const char* fileName, unsigned char** bytes, int* len);

  //----------------------------------------------------------------------------

  static v8::Handle<v8::Value> LoadString(const v8::Arguments& args);
  static v8::Handle<v8::Value> Load(const v8::Arguments& args);

  static v8::Handle<v8::Value> FontFace(const v8::Arguments& args);

  /** args: className, staticMethodName[, stringParam] */
  static v8::Handle<v8::Value> CallJava(const v8::Arguments& args);

  static v8::Handle<v8::Value> SaveCanvasToSystemGallery(const v8::Arguments& args);
};

#endif
