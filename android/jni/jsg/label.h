#ifndef _JSG_LABEL_H_
#define _JSG_LABEL_H_

#include <v8.h>

class Label
{
 public:
  static void InitializeNative(v8::Handle<v8::ObjectTemplate> target);
  static v8::Handle<v8::Value> DrawLabel(const v8::Arguments& args);

 private:
  static void draw(
      unsigned char* argbTarget, int width, int height,
      const char* fontName, int fontSize,
      const char* text, const char* color, int alignx, int aligny);
};

#endif
