#ifndef SYSTEM_WINDOWS_OPENGL32_INCLUDED
#define SYSTEM_WINDOWS_OPENGL32_INCLUDED


#include <system.c>


typedef enum
{
    GL_COLOR_BUFFER_BIT = 0x00004000
}
GL_Clear_Mode;

typedef enum
{
    GL_TRIANGLES = 0x0004
}
GL_Begin_Mode;

typedef enum
{
	GL_RGB  = 0x1907,
	GL_BGR  = 0x80E0,
    GL_RGBA = 0x1908,
    GL_BGRA = 0x80E1
}
GL_Data_Format;

typedef enum
{
    GL_UNSIGNED_BYTE = 0x1401
}
GL_Data_Type;

typedef enum
{
    GL_BLEND = 0x0BE2
}
GL_Option;

typedef enum
{
    GL_SRC_ALPHA           = 0x0302,
    GL_ONE_MINUS_SRC_ALPHA = 0x0303
}
GL_Blend_Factor;

typedef enum
{
    GL_MODELVIEW = 0x1700
}
GL_Matrix_Mode;

import void glDrawPixels (Number32 width, Number32 height, GL_Data_Format format, GL_Data_Type type, Byte* pixels);
import void glRasterPos2f (Rational_Number32 x, Rational_Number32 y);

import void glEnable (GL_Option option);
import void glBlendFunc (GL_Blend_Factor source_factor, GL_Blend_Factor destination_factor);

import void glColor3f (Rational_Number32 red, Rational_Number32 green, Rational_Number32 blue);
import void glVertex3f (Rational_Number32 x, Rational_Number32 y, Rational_Number32 z);
import void glBegin (GL_Begin_Mode mode);
import void glEnd ();
import void glClear (GL_Clear_Mode mode);
import void glClearColor (Rational_Number32 red, Rational_Number32 green, Rational_Number32 blue, Rational_Number32 alpha);

import void glMatrixMode(GL_Matrix_Mode mode);
import void glLoadIdentity();
import void glPushMatrix();
import void glPopMatrix();
import void glLoadMatrixf(Rational_Number32* matrix);

import void glTranslatef (Rational_Number32 x, Rational_Number32 y, Rational_Number32 z);
import void glScalef (Rational_Number32 x, Rational_Number32 y, Rational_Number32 z);
import void glRotatef (Rational_Number32 angle, Rational_Number32 x, Rational_Number32 y, Rational_Number32 z);


import Byte*   wglCreateContext(Byte* context);
import Boolean wglDeleteContext(Byte* context);
import void    wglSwapBuffers(Byte* context);
import Boolean wglMakeCurrent(Byte* device_context, Byte* context);
import Byte*   wglGetCurrentDC();


#endif //SYSTEM_WINDOWS_OPENGL32_INCLUDED