#ifndef SYSTEM_WINDOWS_KERNEL32_INCLUDED
#define SYSTEM_WINDOWS_KERNEL32_INCLUDED


#include <system.c>


//=============================== File ===============================

typedef Bit8* File;

//https://docs.microsoft.com/ru-ru/windows/desktop/api/winbase/ns-winbase-_ofstruct
typedef struct
{
    Bit8  size_struct;
    Bit8  file_on_hard_disk;
    Bit16 error_code;
    Bit16 reserved1;
    Bit16 reserved2;
    Bit8  path_name[256];
}
File_Data;

//https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-openfile
typedef enum
{
    OPEN_FILE_READ           = 0,
    OPEN_FILE_WRITE          = 1,
    OPEN_FILE_READ_AND_WRITE = 2
}
Open_File_Option;

import File OpenFile    (Bit8* path, File_Data* file_data, Open_File_Option options);
import Bit8 CloseHandle (File file);

typedef enum
{
    GENERIC_READ  = 0x80000000,
    GENERIC_WRITE = 0x40000000
}
Create_File_Mode;

typedef enum
{
    DISABLE_ALL_FILE_OPERATION   = 0,
    ENABLE_READ_FILE_OPERATION   = 1,
    ENABLE_WRITE_FILE_OPERATION  = 2,
    ENABLE_DELETE_FILE_OPERATION = 4
}
Enabled_File_Operation_For_Other_Processes;

typedef enum
{
    CREATE_NEW        = 1,
    CREATE_ALWAYS     = 2,
    OPEN_EXISTING     = 3,
    OPEN_ALWAYS       = 4,
    TRUNCATE_EXISTING = 5,
}
Create_File_Action;

typedef enum
{
    ARCHIVE_FILE_ATTRIBUTE   = 32,
    ENCRYPTED_FILE_ATTRIBUTE = 16384,
    NORMAL_FILE_ATTRIBUTE    = 128,
    HIDDEN_FILE_ATTRIBUTE    = 2,
    OFFLINE_FILE_ATTRIBUTE   = 4096,
    READONLY_FILE_ATTRIBUTE  = 1,
    SYSTEM_FILE_ATTRIBUTE    = 4,
    TEMPORARY_FILE_ATTRIBUTE = 256,
    NO_BUFFERING_FILE_ATTRIBUTE = 0x20000000,
}
File_Attribute;

import File CreateFileA(
    Bit8*                                       name,
    Create_File_Mode                            mode,
    Enabled_File_Operation_For_Other_Processes  flags2,
    void*                                       security_attributes, // 0 если не наследуется дочерними процессами
    Create_File_Action                          create_action,
    File_Attribute                              attributes,
    File                                        file
);

import File CreateFileW(
    Bit16*                                      name,
    Create_File_Mode                            mode,
    Enabled_File_Operation_For_Other_Processes  flags2,
    void*                                       security_attributes, // 0 если не наследуется дочерними процессами
    Create_File_Action                          create_action,
    File_Attribute                              attributes,
    File                                        file
);

import Bit8 DeleteFileA(Bit8* path);
import Bit8 DeleteFileW(Bit16* path);

import Bit8 ReadFile(
    File   file,
    Bit8*  buffer,
    Bit32  buffer_length,
    Bit32* bytes_readed,
    Bit8*  overlapped
);

import Bit8 WriteFile (
    File   file,
    Bit8*  data,
    Bit32  data_length,
    Bit32* bytes_writed,
    Bit8*  overlapped
);

typedef enum
{
    BEGIN_FILE_POSITION   = 0,
    CURRENT_FILE_POSITION = 1,
    END_FILE_POSITION     = 2
}
File_Pointer_Position;

//https://docs.microsoft.com/ru-ru/windows/desktop/api/fileapi/nf-fileapi-setfilepointer
import Bit32 SetFilePointer (
    File                   file,
    Bit32                  distance,
    Bit32*                 distance_high,
    File_Pointer_Position  method
);

/*
typedef struct
{
    Bit32 dwLowDateTime;
    Bit32 dwHighDateTime;
}
File_Time;
*/

//https://docs.microsoft.com/ru-ru/windows/desktop/api/fileapi/ns-fileapi-_by_handle_file_information
typedef struct
{
    Bit32  attributes;
    Bit64  creation_time;
    Bit64  last_access_time;
    Bit64  last_write_time;
    Bit32  volume_serial_number;
    Bit32  size_high;
    Bit32  size_low;
    Bit32  number_of_links;
    Bit32  file_index_high;
    Bit32  file_index_low;
}
File_Information;

import Bit32 GetFileInformationByHandle(File file, File_Information* information);

typedef enum
{
    DRIVE_UNKNOWN     = 0,
    DRIVE_NO_ROOT_DIR = 1,
    DRIVE_REMOVABLE   = 2,
    DRIVE_FIXED       = 3,
    DRIVE_REMOTE      = 4,
    DRIVE_CDROM       = 5,
    DRIVE_RAMDISK     = 6
}
Drive_Type;

import Drive_Type GetDriveTypeA(Byte* path);
import Number32 GetLogicalDrives();
import Bit32 GetVolumeInformationA(
    Byte*     root,
    Byte*     name,
    Number32  name_size,
    Number32* serial_number,
    Number32* maximum_file_name_size,
    Number32* file_system_flags,
    Byte*     file_system_name,
    Number32  file_system_name_size
);


typedef struct
{
    Byte a;
}
Find_File_Information;

import Byte* FindFirstFileA(Bit8* path, Find_File_Information* information);

//=============================== Memory =============================

//https://docs.microsoft.com/en-us/windows/desktop/api/memoryapi/nf-memoryapi-virtualalloc
typedef enum
{
    COMMIT_MEMORY = 0x00001000,
    RESERVE_MEMORY = 0x00002000,
    RESET_MEMORY = 0x00080000
    //...
}
Memory_Allocation_Type;

//https://docs.microsoft.com/ru-ru/windows/desktop/Memory/memory-protection-constants
typedef enum
{
    EXECUTE_MEMORY_ACCESS                = 0x10,
    EXECUTE_AND_READ_MEMORY_ACCESS       = 0x20,
    EXECUTE_READ_AND_WRITE_MEMORY_ACCESS = 0x40,
    NO_MEMORY_ACCESS                     = 1,
    GUARD_MEMORY_ACCESS                  = 0x100
    //...
}
Memory_Protection_Option;

import Bit8* VirtualAlloc (Bit8* start_address, Bit32 size, Memory_Allocation_Type type, Memory_Protection_Option protection);

//https://docs.microsoft.com/en-us/windows/desktop/api/memoryapi/nf-memoryapi-virtualfree
typedef enum
{
    RELEASE_MEMORY = 0x00008000
    //...
}
Memory_Free_Type;

import void VirtualFree (Bit8* address, Bit32 size, Memory_Free_Type type);


typedef enum
{
    HEAP_GENERATE_EXCEPTIONS   = 0x00000004,
    HEAP_NO_SERIALIZE          = 0x00000001,
    HEAP_REALLOC_IN_PLACE_ONLY = 0x00000010,
    HEAP_ZERO_MEMORY           = 0x00000008
}
Heap_Flags;

import Bit8* GetProcessHeap ();
import Bit8* HeapAlloc      (Byte* heap, Bit32 flags, Number32 bytes_length);
import void  HeapFree       (Bit8* heap, Bit32 flags, Bit8* bytes);
import Bit8* HeapReAlloc    (Byte* heap, Bit32 flags, Bit8* bytes, Number32 bytes_length);


//=============================== Exceptions =========================

typedef struct
{
    Number32          code;
    Number32          flags;
    struct Exception* next_exception;
    Byte*             address;
    Number32          number_parameters;
    Number32          information[15];
}
Exception;

typedef struct
{
    Number32   ControlWord;
    Number32   StatusWord;
    Number32   TagWord;
    Number32   ErrorOffset;
    Number32   ErrorSelector;
    Number32   DataOffset;
    Number32   DataSelector;
    Byte       RegisterArea[80];
    Number32   Cr0NpxState;
}
Floating_Context;

typedef struct
{
    Number32   ContextFlags;
    Number32   Dr0;
    Number32   Dr1;
    Number32   Dr2;
    Number32   Dr3;
    Number32   Dr6;
    Number32   Dr7;
    Floating_Context FloatSave;
    Number32   SegGs;
    Number32   SegFs;
    Number32   SegEs;
    Number32   SegDs;
    Number32   Edi;
    Number32   Esi;
    Number32   Ebx;
    Number32   Edx;
    Number32   Ecx;
    Number32   Eax;
    Number32   Ebp;
    Number32   Eip;
    Number32   SegCs;
    Number32   EFlags;
    Number32   Esp;
    Number32   SegSs;
    Byte       ExtendedRegisters[512];
}
Context;

typedef struct
{
    Exception* exception;
    Context*   context;
}
Exceptions_List;

//https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler
import Number32 AddVectoredExceptionHandler(Number32 priority, void(*handler)(Exceptions_List* exceptions));


//=============================== Thread =============================

typedef struct
{
    Number32  length;
    Bit8*     security_descriptor;
    Bit8      is_inherit_handle;
}
Windows_Sequrity_Attributes;


import void  Sleep        (Bit32 milliseconds);
import Bit32 CreateThread (
    Windows_Sequrity_Attributes* security_attributes,
    Number32                     stack_size,
    void                       (*thread_function)(Bit8* arguments),
    Bit8*                        attributes,
    Bit32                        creation_flags,
    Bit32*                       thread_id
);


//=============================== Console ============================


typedef enum
{
    STD_INPUT_HANDLE   = -10,
    STD_OUTPUT_HANDLE  = -11,
    STD_ERROR_HANDLE   = -12
}
Std_Handle;

import Byte* GetStdHandle(Std_Handle handle);

typedef enum
{
    FOREGROUND_BLUE            = 0x0001,
    FOREGROUND_GREEN           = 0x0002,
    FOREGROUND_RED             = 0x0004,
    FOREGROUND_INTENSITY       = 0x0008,
    BACKGROUND_BLUE            = 0x0010,
    BACKGROUND_GREEN           = 0x0020,
    BACKGROUND_RED             = 0x0040,
    BACKGROUND_INTENSITY       = 0x0080,
    COMMON_LVB_LEADING_BYTE    = 0x0100,
    COMMON_LVB_TRAILING_BYTE   = 0x0200,
    COMMON_LVB_GRID_HORIZONTAL = 0x0400,
    COMMON_LVB_GRID_LVERTICAL  = 0x0800,
    COMMON_LVB_GRID_RVERTICAL  = 0x1000,
    COMMON_LVB_REVERSE_VIDEO   = 0x4000,
    COMMON_LVB_UNDERSCORE      = 0x8000
}
Console_Attribute;

import Bit32 SetConsoleTextAttribute(Byte* console, Number16 attribute);

import Bit32 WriteConsoleA(
    Byte*     console_handle,
    Byte*     charachters,
    Number32  charachters_length,
    Number32* charachters_written,
    Byte*     reserved
);


//=============================== Library ============================


import Byte* LoadLibraryA   (Byte* file_name);
import Byte* GetProcAddress (Byte* library, Byte* procedure_name);


#endif //SYSTEM_WINDOWS_KERNEL32_INCLUDED