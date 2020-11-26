#ifndef SYSTEM_WINDOWS_WINMM_INCLUDED
#define SYSTEM_WINDOWS_WINMM_INCLUDED


#include <system.c>


#define CALLBACK_THREAD     0x20000
#define CALLBACK_FUNCTION   0x30000
#define CALLBACK_EVENT      0x50000
#define WHDR_BEGINLOOP      4
#define MM_WOM_DONE         0x3BD

#define PCM_WAVE_FORMAT     1

typedef struct
{
    Bit16    format;
    Number16 number_of_channels;
    Number32 samples_per_seconds;
    Number32 bytes_per_seconds;
    Number16 block_align;
    Number16 bits_per_sample;
    Number16 size_of_appended_information;
}
Windows_Sound_Format;

typedef struct Windows_Sound_Buffer
{
    Byte*                        data;
    Number32                     buffer_length;
    Number32                     bytes_recorded;
    Byte*                        user;
    Number32                     flags;
    Number32                     loops;
    struct Windows_Sound_Buffer* next;
    Byte*                        reserved;
}
Windows_Sound_Buffer;


import Number32 waveOutOpen(
    Byte*                 sound_device,
    Number32              device_ID,
    Windows_Sound_Format* format,
    void                (*on_buffer_end)(Byte *sound_device, Byte *message, Byte *arguments, Byte *parameter1, Byte *parameter2),
    Byte*                 arguments,
    Bit32                 flags
);

import Bit32 waveOutClose(Byte* sound_device);

import Bit32 waveOutPrepareHeader(
    Byte*                 sound_device,
    Windows_Sound_Buffer* buffer,
    Number32              size_of_struct_buffer
);

import Bit32 waveOutWrite(
    Byte*                 sound_device,
    Windows_Sound_Buffer* buffer,
    Number32              size_of_struct_buffer
);


#endif //SYSTEM_WINDOWS_WINMM_INCLUDED