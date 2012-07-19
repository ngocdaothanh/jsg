#ifndef _FONT_FACE_H_
#define _FONT_FACE_H_

#include <map>
#include <string>
#include <cairo-ft.h>

class FontFace
{
 public:
  static bool load(const char* name, const unsigned char* bytes, int len);
  static cairo_font_face_t* get(const char* name);

 private:
  static std::map<std::string, cairo_font_face_t*> storage;
};

#endif
