#ifndef _JSG_LOG_H_
#define _JSG_LOG_H_

#include <android/log.h>
#include <node_object_wrap.h>

#define JSG_LOG_TAG "JSG"
#define LOGI(...)   __android_log_print(ANDROID_LOG_INFO,  JSG_LOG_TAG, __VA_ARGS__)
#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR, JSG_LOG_TAG, __VA_ARGS__)

class Console: public node::ObjectWrap
{
 public:
  static void InitializeNative(v8::Handle<v8::ObjectTemplate> target);
  static void LoadScript();

  static v8::Persistent<v8::FunctionTemplate> constructor;
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Log(const v8::Arguments& args);
};

#endif
