#ifndef SYSTEM_WINDOWS_GDI32_INCLUDED
#define SYSTEM_WINDOWS_GDI32_INCLUDED


#include <system.c>


typedef struct
{
    struct
    {
        Number32         size_of_structure;
        Integer_Number32 width;
        Integer_Number32 height;
        Number16         planes;
        Number16         bit_count;
        Number32         compression;
        Number32         size_image;
        Integer_Number32 x_pels_per_meter;
        Integer_Number32 y_pels_per_meter;
        Number32         number_of_color_indexes;
        Number32         number_of_color_indexes_important;
    }
    header;
    Number32 color[1];
}
Bitmap_Info;


import Integer_Number32     SetDIBitsToDevice(
    Byte*            device_context,
    Integer_Number32 x,
    Integer_Number32 y,
    Number32         width,
    Number32         height,
    Integer_Number32 x_image,
    Integer_Number32 y_image,
    Number32         start_scan,
    Number32         scan_lines_length,
    Byte*            image,
    Bitmap_Info*     bitmap_info,
    Number32         color_type
);


typedef struct
{
    Bit16  size;
    Bit16  nVersion;
    Bit32  dwFlags;
    Bit8   iPixelType;
    Bit8   cColorBits;
    Bit8   cRedBits;
    Bit8   cRedShift;
    Bit8   cGreenBits;
    Bit8   cGreenShift;
    Bit8   cBlueBits;
    Bit8   cBlueShift;
    Bit8   cAlphaBits;
    Bit8   cAlphaShift;
    Bit8   cAccumBits;
    Bit8   cAccumRedBits;
    Bit8   cAccumGreenBits;
    Bit8   cAccumBlueBits;
    Bit8   cAccumAlphaBits;
    Bit8   cDepthBits;
    Bit8   cStencilBits;
    Bit8   cAuxBuffers;
    Bit8   iLayerType;
    Bit8   bReserved;
    Bit32  dwLayerMask;
    Bit32  dwVisibleMask;
    Bit32  dwDamageMask;
}
Windows_Pixel_Format_Descriptor;

#define PFD_DRAW_TO_WINDOW  4
#define PFD_SUPPORT_OPENGL  32
#define PFD_DOUBLEBUFFER    1
#define PFD_TYPE_RGBA   0
#define PFD_MAIN_PLANE  0


import Number32 ChoosePixelFormat(Byte* context, Windows_Pixel_Format_Descriptor* format_descriptor);
import Bit32    SetPixelFormat(Byte* context, Number32 format, Windows_Pixel_Format_Descriptor* format_descriptor);

import Byte*    CreateSolidBrush(Number32 color);

import Bit32    SwapBuffers(Byte* device);

import Bit32    TextOutA(Byte* device_context, Number32 x, Number32 y, Byte* text, Number32 text_length);
import Bit32    TextOutW(Byte* device_context, Number32 x, Number32 y, Bit16* text, Number32 text_length);


#endif //SYSTEM_WINDOWS_GDI32_INCLUDED