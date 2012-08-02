#ifndef _JSG_PREFS_H_
#define _JSG_PREFS_H_

#include <v8.h>

class Prefs
{
 public:
  static void InitializeNative(v8::Handle<v8::ObjectTemplate> target);

  static v8::Handle<v8::Value> GetString(const v8::Arguments& args);
  static v8::Handle<v8::Value> SetString(const v8::Arguments& args);

  static v8::Handle<v8::Value> GetInt(const v8::Arguments& args);
  static v8::Handle<v8::Value> SetInt(const v8::Arguments& args);
};

#endif
