#ifndef _JSG_GL_H_
#define _JSG_GL_H_

// Some devices requires that texture size is power of 2
// http://quirkygba.blogspot.jp/2011/04/why-are-my-textures-not-showing-up-on.html
class GL
{
 private:
  static unsigned int program;
  static int          projection;
  static int          position;
  static int          texCoord;
  static int          sampler;

  static int            width;
  static int            height;
  static int            textureWidth;
  static int            textureHeight;
  static unsigned char* rgba;
  static unsigned int   stageTextureId;
  static float          vertices[20];

 public:
  static void setupGraphics(int width, int height);
  static void draw(unsigned char* rgba, int width, int height);

 private:
  static int  nearestPow2(int n);
  static void resize(int width, int height);
  static void argb2rgba(unsigned char* argb);
};

#endif
