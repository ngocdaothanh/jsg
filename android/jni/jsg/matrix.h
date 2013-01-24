#ifndef _JSG_MATRIX_H_
#define _JSG_MATRIX_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;

void esMatrixLoadIdentity(ESMatrix *result);
void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB);
void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);

#endif
