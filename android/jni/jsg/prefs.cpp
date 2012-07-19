#include "prefs.h"
#include "jsg.h"

using namespace v8;

void Prefs::InitializeNative(Handle<ObjectTemplate> target)
{
  target->Set(String::New("jsgPrefsGetString"), FunctionTemplate::New(Prefs::GetString));
  target->Set(String::New("jsgPrefsSetString"), FunctionTemplate::New(Prefs::SetString));
}

Handle<Value> Prefs::GetString(const Arguments& args)
{
  HandleScope scope;

  String::Utf8Value vKey(args[0]);
  const char* cKey = *vKey;

  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/Prefs");
    mid   = env->GetStaticMethodID(klass, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
  }

  jstring jKey   = env->NewStringUTF(cKey);
  jstring jValue = (jstring) env->CallStaticObjectMethod(klass, mid, jKey);
  env->DeleteLocalRef(jKey);

  if (jValue) {
    const char*  cValue = env->GetStringUTFChars(jValue, NULL);
    int          len    = env->GetStringUTFLength(jValue);
    Local<Value> vValue = String::New(cValue, len);

    env->ReleaseStringUTFChars(jValue, cValue);
    env->DeleteLocalRef(jValue);
    return scope.Close(vValue);
  } else {
    return Null();
  }
}

Handle<Value> Prefs::SetString(const Arguments& args)
{
  HandleScope scope;

  String::Utf8Value vKey  (args[0]);
  String::Utf8Value vValue(args[1]);

  const char* cKey   = *vKey;
  const char* cValue = *vValue;

  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/Prefs");
    mid   = env->GetStaticMethodID(klass, "setString", "(Ljava/lang/String;Ljava/lang/String;)V");
  }

  jstring jKey   = env->NewStringUTF(cKey);
  jstring jValue = env->NewStringUTF(cValue);
  env->CallStaticVoidMethod(klass, mid, jKey, jValue);
  env->DeleteLocalRef(jKey);
  env->DeleteLocalRef(jValue);

  return Undefined();
}
