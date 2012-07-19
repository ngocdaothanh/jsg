#include "FontFace.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "../jsg/log.h"

std::map<std::string, cairo_font_face_t*> FontFace::storage;

bool FontFace::load(const char* name, const unsigned char* bytes, int len)
{
  std::string key(name);
  if (storage[key]) return true;

  int error;

  static FT_Library library;
  static bool inited = false;
  if (!inited) {
    error = FT_Init_FreeType(&library);
    if (error) return false;
    inited = true;
  }

  FT_Face face;
  error = FT_New_Memory_Face(library, bytes, len, 0, &face);
  if (error) return false;

  cairo_font_face_t* font_face = cairo_ft_font_face_create_for_ft_face(face, 0);
  cairo_ft_font_face_set_synthesize(font_face, CAIRO_FT_SYNTHESIZE_BOLD | CAIRO_FT_SYNTHESIZE_OBLIQUE);

  storage[key] = font_face;
  return true;
}

cairo_font_face_t* FontFace::get(const char* name)
{
  std::string key(name);
  return storage[key];
}
