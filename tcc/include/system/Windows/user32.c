#ifndef SYSTEM_WINDOWS_USER32_INCLUDED
#define SYSTEM_WINDOWS_USER32_INCLUDED


#include <system.c>


//https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types
//https://github.com/tpn/winsdk-10/blob/master/Include/10.0.10240.0/um/WinUser.h


#define UNIQUE_FOR_EACH_WINDOW_CONTEXT_CLASS_STYLE  32

//#define CLOSE_WINDOW_MESSAGE                        16
//#define DESTROY_WINDOW_MESSAGE                      2
//#define PAINT_WINDOW_MESSAGE                        15
//#define ERASE_BACKGROUND_MESSAGE                    20

#define DEFAULT_ICON                                32512
#define DEFAULT_ARROW                               32512

//#define POPUP_WINDOW_STYLE                          0x80000000

#define REMOVED_FROM_QUEUE_MESSAGE                  1
#define QUIT_WINDOW_MESSAGE                         0x0012
#define UPDATE_NOW                                  256

#define USER_DATA                                   -21


typedef struct
{

}
Icon;

typedef struct
{

}
Cursor;

typedef struct
{

}
Brush;

typedef struct
{
    Number32    structure_size;
    Number32    style;
    Number32  (*window_procedure)();
    Number32    class;
    Byte*       window;
    Number16*   instance;
    Icon*       icon;
    Cursor*     cursor;
    Brush*      background;
    Byte*       menu_name;
    Byte*       class_name;
    Icon*       small_icon;
}
Window_Class;

typedef struct
{
    Integer_Number32 left;
    Integer_Number32 top;
    Integer_Number32 right;
    Integer_Number32 bottom;
}
Windows_Rectangle;

typedef struct
{
    Byte*              context;
    Integer_Number32   erase_background;
    Windows_Rectangle  rectangle;
    Integer_Number32   restore;
    Integer_Number32   update;
    Byte               color[32];
}
Windows_Paint;

typedef struct
{
    Integer_Number32 x;
    Integer_Number32 y;
}
Windows_Point;

typedef struct
{
    Byte*           window;
    Number32        message;
    Number*         parameter1;
    Integer_Number* parameter2;
    Number32        time;
    Windows_Point   point;
}
Windows_Message;

import Number16 RegisterClassExA (Window_Class* class);
import Number16 RegisterClassExW (Window_Class* class);

typedef enum
{
    GCL_CBCLSEXTRA    = -20,
    GCL_CBWNDEXTRA    = -18,
    GCL_HBRBACKGROUND = -10,
    GCL_HCURSOR       = -12,
    GCL_HICON         = -14,
    GCL_HICONSM       = -34,
    GCL_HMODULE       = -16,
    GCL_MENUNAME      = -8,
    GCL_STYLE         = -26,
    GCL_WNDPROC       = -24
}
Class_Index;

import Bit32 SetClassLongA (Byte* window, Class_Index index, Number32 value);

//https://github.com/AHK-just-me/AHK_Gui_Constants/tree/master/Sources
//https://github.com/forcedotcom/dataloader/blob/master/windows-dependencies/autoit/Include/ButtonConstants.au3
typedef enum
{
    WS_OVERLAPPED   = 0x00000000L,
    WS_MAXIMIZEBOX  = 0x00010000L,
    WS_TABSTOP      = 0x00010000L,
    WS_MINIMIZEBOX  = 0x00020000L,
    WS_GROUP        = 0x00020000L,
    WS_SIZEBOX      = 0x00040000L,
    WS_THICKFRAME   = 0x00040000L,
    WS_SYSMENU      = 0x00080000L,
    WS_HSCROLL      = 0x00100000L,
    WS_VSCROLL      = 0x00200000L,
    WS_DLGFRAME     = 0x00400000L,
    WS_BORDER       = 0x00800000L,
    WS_CAPTION      = 0x00C00000L,
    WS_MAXIMIZE     = 0x01000000L,
    WS_CLIPCHILDREN = 0x02000000L,
    WS_CLIPSIBLINGS = 0x04000000L,
    WS_DISABLED     = 0x08000000L,
    WS_VISIBLE      = 0x10000000L,
    WS_MINIMIZE     = 0x20000000L,
    WS_ICONIC       = 0x20000000L,
    WS_CHILD        = 0x40000000L,
    WS_CHILDWINDOW  = 0x40000000L,
    WS_POPUP        = 0x80000000L,

    // Static
    SS_LEFT           = 0x0000,
    SS_CENTER         = 0x0001,
    SS_RIGHT          = 0x0002,
    SS_ICON           = 0x0003,
    SS_BLACKRECT      = 0x0004,
    SS_GRAYRECT       = 0x0005,
    SS_WHITERECT      = 0x0006,
    SS_BLACKFRAME     = 0x0007,
    SS_GRAYFRAME      = 0x0008,
    SS_WHITEFRAME     = 0x0009,
    SS_SIMPLE         = 0x000B,
    SS_LEFTNOWORDWRAP = 0x000C,
    SS_BITMAP         = 0x000E,
    SS_ETCHEDHORZ     = 0x0010,
    SS_ETCHEDVERT     = 0x0011,
    SS_ETCHEDFRAME    = 0x0012,
    SS_NOPREFIX       = 0x0080,
    SS_NOTIFY         = 0x0100,
    SS_CENTERIMAGE    = 0x0200,
    SS_RIGHTJUST      = 0x0400,
    SS_SUNKEN         = 0x1000,

    // Button
    BS_PUSHBUTTON        = 0x0000,
    BS_DEFPUSHBUTTON     = 0x0001,
    BS_CHECKBOX          = 0x0002,
    BS_AUTOCHECKBOX      = 0x0003,
    BS_RADIOBUTTON       = 0x0004,
    BS_3STATE            = 0x0005,
    BS_AUTO3STATE        = 0x0006,
    BS_GROUPBOX          = 0x0007,
    BS_USERBUTTON        = 0x0008,
    BS_AUTORADIOBUTTON   = 0x0009,
    BS_PUSHBOX           = 0x000A,
    BS_OWNERDRAW         = 0x000B,
    BS_SPLITBUTTON       = 0x000C, // >= Vista
    BS_DEFSPLITBUTTON    = 0x000D, // >= Vista
    BS_COMMANDLINK       = 0x000E, // >= Vista
    BS_DEFCOMMANDLINK    = 0x000F, // >= Vista
    BS_RIGHTBUTTON       = 0x0020, // BS_LEFTTEXT
    BS_ICON              = 0x0040,
    BS_BITMAP            = 0x0080,
    BS_LEFT              = 0x0100,
    BS_RIGHT             = 0x0200,
    BS_CENTER            = 0x0300,
    BS_TOP               = 0x0400,
    BS_BOTTOM            = 0x0800,
    BS_VCENTER           = 0x0C00,
    BS_PUSHLIKE          = 0x1000,
    BS_MULTILINE         = 0x2000,
    BS_NOTIFY            = 0x4000,
    BS_FLAT              = 0x8000,

    // Edit text
    ES_LEFT                = 0x0000,
    ES_CENTER              = 0x0001,
    ES_RIGHT               = 0x0002,
    ES_MULTILINE           = 0x0004,
    ES_UPPERCASE           = 0x0008,
    ES_LOWERCASE           = 0x0010,
    ES_PASSWORD            = 0x0020,
    ES_AUTOVSCROLL         = 0x0040,
    ES_AUTOHSCROLL         = 0x0080,
    ES_NOHIDESEL           = 0x0100,
    ES_OEMCONVERT          = 0x0400,
    ES_READONLY            = 0x0800,
    ES_WANTRETURN          = 0x1000,
    ES_NUMBER              = 0x2000
}
Window_Style;

typedef enum
{
    // Window messages
    WM_GETTEXTLENGTH = 0x000E,
    WM_GETTEXT       = 0x000D,
    WM_SIZE          = 0x0005,
    WM_SIZING        = 0x0214,
    WM_USER          = 0X0400,

    WM_CREATE          = 0x0001,
    WM_DESTROY         = 0x0002,
    WM_MOVE            = 0x0003,
    WM_ACTIVATE        = 0x0006,
    WM_SETFOCUS        = 0x0007,
    WM_KILLFOCUS       = 0x0008,
    WM_ENABLE          = 0x000A,
    WM_SETREDRAW       = 0x000B,
    WM_SETTEXT         = 0x000C,
    WM_PAINT           = 0x000F,
    WM_CLOSE           = 0x0010,
    WM_QUERYENDSESSION = 0x0011,
    WM_QUIT            = 0x0012,
    WM_QUERYOPEN       = 0x0013,
    WM_ERASEBKGND      = 0x0014,
    WM_SYSCOLORCHANGE  = 0x0015,
    WM_ENDSESSION      = 0x0016,
    WM_SHOWWINDOW      = 0x0018,

    WM_SETTINGCHANGE   = 0x001A,
    WM_WININICHANGE    = 0x001A,
    WM_DEVMODECHANGE   = 0x001B,
    WM_ACTIVATEAPP     = 0x001C,
    WM_FONTCHANGE      = 0x001D,
    WM_TIMECHANGE      = 0x001E,
    WM_CANCELMODE      = 0x001F,

    WM_IME_STARTCOMPOSITION = 0x010D,
    WM_IME_ENDCOMPOSITION   = 0x010E,
    WM_IME_COMPOSITION      = 0x010F,

    WM_SETCURSOR              = 0x0020,
    WM_MOUSEACTIVATE          = 0x0021,
    WM_CHILDACTIVATE          = 0x0022,
    WM_QUEUESYNC              = 0x0023,
    WM_GETMINMAXINFO          = 0x0024,
    WM_PAINTICON              = 0x0026,
    WM_ICONERASEBKGND         = 0x0027,
    WM_NEXTDLGCTL             = 0x0028,
    WM_SPOOLERSTATUS          = 0x002A,
    WM_DRAWITEM               = 0x002B,
    WM_MEASUREITEM            = 0x002C,
    WM_DELETEITEM             = 0x002D,
    WM_VKEYTOITEM             = 0x002E,
    WM_CHARTOITEM             = 0x002F,
    WM_SETFONT                = 0x0030,
    WM_GETFONT                = 0x0031,
    WM_SETHOTKEY              = 0x0032,
    WM_GETHOTKEY              = 0x0033,
    WM_QUERYDRAGICON          = 0x0037,
    WM_COMPAREITEM            = 0x0039,
    WM_GETOBJECT              = 0x003D,
    WM_COMPACTING             = 0x0041,
    WM_COMMNOTIFY             = 0x0044,
    WM_WINDOWPOSCHANGING      = 0x0046,
    WM_WINDOWPOSCHANGED       = 0x0047,
    WM_POWER                  = 0x0048,
    WM_NOTIFY                 = 0x004E,
    WM_COPYDATA               = 0x004A,
    WM_CANCELJOURNAL          = 0x004B,
    WM_INPUTLANGCHANGEREQUEST = 0x0050,
    WM_INPUTLANGCHANGE        = 0x0051,
    WM_TCARD                  = 0x0052,
    WM_HELP                   = 0x0053,
    WM_USERCHANGED            = 0x0054,
    WM_NOTIFYFORMAT           = 0x0055,

    WM_PARENTNOTIFY        = 0x0210,
    WM_ENTERMENULOOP       = 0x0211,
    WM_EXITMENULOOP        = 0x0212,
    WM_NEXTMENU            = 0x0213,
    WM_CAPTURECHANGED      = 0x0215,
    WM_MOVING              = 0x0216,
    WM_POWERBROADCAST      = 0x0218,
    WM_DEVICECHANGE        = 0x0219,
    WM_MDICREATE           = 0x0220,
    WM_MDIDESTROY          = 0x0221,
    WM_MDIACTIVATE         = 0x0222,
    WM_MDIRESTORE          = 0x0223,
    WM_MDINEXT             = 0x0224,
    WM_MDIMAXIMIZE         = 0x0225,
    WM_MDITILE             = 0x0226,
    WM_MDICASCADE          = 0x0227,
    WM_MDIICONARRANGE      = 0x0228,
    WM_MDIGETACTIVE        = 0x0229,
    WM_MDISETMENU          = 0x0230,
    WM_ENTERSIZEMOVE       = 0x0231,
    WM_EXITSIZEMOVE        = 0x0232,
    WM_DROPFILES           = 0x0233,
    WM_MDIREFRESHMENU      = 0x0234,
    WM_IME_SETCONTEXT      = 0x0281,
    WM_IME_NOTIFY          = 0x0282,
    WM_IME_CONTROL         = 0x0283,
    WM_IME_COMPOSITIONFULL = 0x0284,
    WM_IME_SELECT          = 0x0285,
    WM_IME_CHAR            = 0x0286,
    WM_IME_REQUEST         = 0x0288,
    WM_IME_KEYDOWN         = 0x0290,
    WM_IME_KEYUP           = 0x0291,
    WM_NCMOUSEHOVER        = 0x02A0,
    WM_MOUSEHOVER          = 0x02A1,
    WM_NCMOUSELEAVE        = 0x02A2,
    WM_MOUSELEAVE          = 0x02A3,
    WM_WTSSESSION_CHANGE   = 0x02B1,
    WM_TABLET_FIRST        = 0x02C0,
    WM_TABLET_LAST         = 0x02DF,

    WM_CUT               = 0x0300,
    WM_COPY              = 0x0301,
    WM_PASTE             = 0x0302,
    WM_CLEAR             = 0x0303,
    WM_UNDO              = 0x0304,
    WM_PALETTEISCHANGING = 0x0310,
    WM_HOTKEY            = 0x0312,
    WM_PALETTECHANGED    = 0x0311,
    WM_PRINT             = 0x0317,
    WM_PRINTCLIENT       = 0x0318,
    WM_APPCOMMAND        = 0x0319,
    WM_QUERYNEWPALETTE   = 0x030F,
    WM_THEMECHANGED      = 0x031A,
    WM_HANDHELDFIRST     = 0x0358,
    WM_HANDHELDLAST      = 0x035F,
    WM_AFXFIRST          = 0x0360,
    WM_AFXLAST           = 0x037F,
    WM_PENWINFIRST       = 0x0380,
    WM_PENWINLAST        = 0x038F,

    WM_CONTEXTMENU   = 0x007B,
    WM_STYLECHANGING = 0x007C,
    WM_STYLECHANGED  = 0x007D,
    WM_DISPLAYCHANGE = 0x007E,
    WM_GETICON       = 0x007F,
    WM_SETICON       = 0x0080,
    WM_NCCREATE      = 0x0081,
    WM_NCDESTROY     = 0x0082,
    WM_NCCALCSIZE    = 0x0083,
    WM_NCHITTEST     = 0x0084,
    WM_NCPAINT       = 0x0085,
    WM_NCACTIVATE    = 0x0086,
    WM_GETDLGCODE    = 0x0087,
    WM_SYNCPAINT     = 0x0088,

    WM_NCMOUSEMOVE     = 0x00A0,
    WM_NCLBUTTONDOWN   = 0x00A1,
    WM_NCLBUTTONUP     = 0x00A2,
    WM_NCLBUTTONDBLCLK = 0x00A3,
    WM_NCRBUTTONDOWN   = 0x00A4,
    WM_NCRBUTTONUP     = 0x00A5,
    WM_NCRBUTTONDBLCLK = 0x00A6,
    WM_NCMBUTTONDOWN   = 0x00A7,
    WM_NCMBUTTONUP     = 0x00A8,
    WM_NCMBUTTONDBLCLK = 0x00A9,
    WM_NCXBUTTONDOWN   = 0x00AB,
    WM_NCXBUTTONUP     = 0x00AC,
    WM_NCXBUTTONDBLCLK = 0x00AD,

    WM_KEYDOWN     = 0x0100,
    WM_KEYFIRST    = 0x0100,
    WM_KEYUP       = 0x0101,
    WM_CHAR        = 0x0102,
    WM_DEADCHAR    = 0x0103,
    WM_SYSKEYDOWN  = 0x0104,
    WM_SYSKEYUP    = 0x0105,
    WM_SYSCHAR     = 0x0106,
    WM_SYSDEADCHAR = 0x0107,
    WM_KEYLAST     = 0x0109,
    WM_UNICHAR     = 0x0109,

    WM_INITDIALOG        = 0x0110,
    WM_COMMAND           = 0x0111,
    WM_SYSCOMMAND        = 0x0112,
    WM_TIMER             = 0x0113,
    WM_HSCROLL           = 0x0114,
    WM_VSCROLL           = 0x0115,
    WM_INITMENU          = 0x0116,
    WM_INITMENUPOPUP     = 0x0117,
    WM_MENUSELECT        = 0x011F,
    WM_MENUCHAR          = 0x0120,
    WM_ENTERIDLE         = 0x0121,
    WM_MENURBUTTONUP     = 0x0122,
    WM_MENUDRAG          = 0x0123,
    WM_MENUGETOBJECT     = 0x0124,
    WM_UNINITMENUPOPUP   = 0x0125,
    WM_MENUCOMMAND       = 0x0126,
    WM_CHANGEUISTATE     = 0x0127,
    WM_UPDATEUISTATE     = 0x0128,
    WM_QUERYUISTATE      = 0x0129,
    WM_CTLCOLORMSGBOX    = 0x0132,
    WM_CTLCOLOREDIT      = 0x0133,
    WM_CTLCOLORLISTBOX   = 0x0134,
    WM_CTLCOLORBTN       = 0x0135,
    WM_CTLCOLORDLG       = 0x0136,
    WM_CTLCOLORSCROLLBAR = 0x0137,
    WM_CTLCOLORSTATIC    = 0x0138,
    WM_CTLCOLOR          = 0x0019,

    // Button messages
    WM_MOUSEMOVE        = 0x0200,
    WM_LBUTTONDOWN      = 0x0201,
    WM_LBUTTONUP        = 0x0202,
    WM_LBUTTONDBLCLK    = 0x0203,
    WM_RBUTTONDOWN      = 0x0204,
    WM_RBUTTONUP        = 0x0205,
    WM_RBUTTONDBLCLK    = 0x0206,
    WM_MBUTTONDOWN      = 0x0207,
    WM_MBUTTONUP        = 0x0208,
    WM_MBUTTONDBLCLK    = 0x0209,
    WM_MOUSEWHEEL       = 0x020A,

    // Edit text messages
    EM_CANUNDO             = 0x00C6,
    EM_CHARFROMPOS         = 0x00D7,
    EM_EMPTYUNDOBUFFER     = 0x00CD,
    EM_FMTLINES            = 0x00C8,
    EM_GETCUEBANNER        = 0x1502, // (ECM_FIRST + 2) Unicode
    EM_GETFIRSTVISIBLELINE = 0x00CE,
    EM_GETHANDLE           = 0x00BD,
    EM_GETHILITE           = 0x1506, // (ECM_FIRST + 6) Unicode >= Vista, not documented
    EM_GETIMESTATUS        = 0x00D9,
    EM_GETLIMITTEXT        = 0x00D5,
    EM_GETLINE             = 0x00C4,
    EM_GETLINECOUNT        = 0x00BA,
    EM_GETMARGINS          = 0x00D4,
    EM_GETMODIFY           = 0x00B8,
    EM_GETPASSWORDCHAR     = 0x00D2,
    EM_GETRECT             = 0x00B2,
    EM_GETSEL              = 0x00B0,
    EM_GETTHUMB            = 0x00BE,
    EM_GETWORDBREAKPROC    = 0x00D1,
    EM_HIDEBALLOONTIP      = 0x1504, // (ECM_FIRST + 4) Unicode
    EM_LIMITTEXT           = 0x00C5,
    EM_LINEFROMCHAR        = 0x00C9,
    EM_LINEINDEX           = 0x00BB,
    EM_LINELENGTH          = 0x00C1,
    EM_LINESCROLL          = 0x00B6,
    EM_POSFROMCHAR         = 0x00D6,
    EM_REPLACESEL          = 0x00C2,
    EM_SCROLL              = 0x00B5,
    EM_SCROLLCARET         = 0x00B7,
    EM_SETCUEBANNER        = 0x1501, // (ECM_FIRST + 1) Unicode
    EM_SETHANDLE           = 0x00BC,
    EM_SETHILITE           = 0x1505, // (ECM_FIRST + 5) Unicode >= Vista, not documented
    EM_SETIMESTATUS        = 0x00D8,
    EM_SETLIMITTEXT        = 0x00C5, // EM_LIMITTEXT
    EM_SETMARGINS          = 0x00D3,
    EM_SETMODIFY           = 0x00B9,
    EM_SETPASSWORDCHAR     = 0x00CC,
    EM_SETREADONLY         = 0x00CF,
    EM_SETRECT             = 0x00B3,
    EM_SETRECTNP           = 0x00B4,
    EM_SETSEL              = 0x00B1,
    EM_SETTABSTOPS         = 0x00CB,
    EM_SETWORDBREAKPROC    = 0x00D0,
    EM_SHOWBALLOONTIP      = 0x1503, // (ECM_FIRST + 2) Unicode
    EM_UNDO                = 0x00C7
}
Window_Message;


import Byte* CreateWindowExA(
    Number32          extended_style,
    Byte*             class_name,
    Byte*             window_name,
    Window_Style      style,
    Integer_Number32  x,
    Integer_Number32  y,
    Integer_Number32  width,
    Integer_Number32  height,
    Byte*             parent_window,
    Byte*             menu,
    Byte*             instance,
    Number32*         parameters
);
import Byte* CreateWindowExW(
    Number32          extended_style,
    Bit16*            class_name,
    Bit16*            window_name,
    Window_Style      style,
    Integer_Number32  x,
    Integer_Number32  y,
    Integer_Number32  width,
    Integer_Number32  height,
    Byte*             parent_window,
    Byte*             menu,
    Byte*             instance,
    Number32*         parameters
);
import Bit32 DestroyWindow(Byte* window);
import Byte* GetDesktopWindow();

typedef enum
{
    SHOW_NORMAL_WINDOW    = 1,
    SHOW_MAXIMIZED_WINDOW = 3,
    SHOW_WINDOW           = 5,
    SHOW_DEFAULT_WINDOW   = 10,
    SHOW_MINIMIZED_WINDOW = 11
}
Show_Window_Type;

typedef enum
{
    GWL_EXSTYLE   = -20,
    GWL_STYLE     = -16,
    GWL_WNDPROC   = -4,
    GWL_HINSTANCE = -6
}
Window_Field_Index;

import Number32 ShowWindow       (Byte* window, Show_Window_Type type);
import Number32 UpdateWindow     (Byte* window);
import Number32 RedrawWindow     (Byte* window, Windows_Rectangle* region, Byte* update_region, Number32 flags);
import Byte*    GetDC            (Byte* window);
import Number32 SetWindowLongA   (Byte* window, Number32 field_index, Number32 value);
import Number32 SetWindowLongW   (Byte* window, Number32 field_index, Number32 value);
import Number32 GetWindowLongA   (Byte* window, Number32 field_index);
import Number32 GetWindowLongW   (Byte* window, Number32 field_index);
import Number32 DefWindowProcA   (Byte* window, Number32 message, Number16* parameter1, Number32* parameter2);

typedef enum
{
    CTLCOLOR_MSGBOX         = 0,
    CTLCOLOR_EDIT           = 1,
    CTLCOLOR_LISTBOX        = 2,
    CTLCOLOR_BTN            = 3,
    CTLCOLOR_DLG            = 4,
    CTLCOLOR_SCROLLBAR      = 5,
    CTLCOLOR_STATIC         = 6,
    CTLCOLOR_MAX            = 7,

    COLOR_SCROLLBAR           = 0,
    COLOR_BACKGROUND          = 1,
    COLOR_ACTIVECAPTION       = 2,
    COLOR_INACTIVECAPTION     = 3,
    COLOR_MENU                = 4,
    COLOR_WINDOW              = 5,
    COLOR_WINDOWFRAME         = 6,
    COLOR_MENUTEXT            = 7,
    COLOR_WINDOWTEXT          = 8,
    COLOR_CAPTIONTEXT         = 9,
    COLOR_ACTIVEBORDER        = 10,
    COLOR_INACTIVEBORDER      = 11,
    COLOR_APPWORKSPACE        = 12,
    COLOR_HIGHLIGHT           = 13,
    COLOR_HIGHLIGHTTEXT       = 14,
    COLOR_BTNFACE             = 15,
    COLOR_BTNSHADOW           = 16,
    COLOR_GRAYTEXT            = 17,
    COLOR_BTNTEXT             = 18,
    COLOR_INACTIVECAPTIONTEXT = 19,
    COLOR_BTNHIGHLIGHT        = 20
}
Windows_Color;

import Byte*    BeginPaint       (Byte* window, Windows_Paint* paint);
import Number32 FillRect         (Byte* device, Windows_Rectangle* rectangle, Byte* brush);
import Number32 EndPaint         (Byte* window, Windows_Paint* paint);

import Icon*    LoadIconA   (Byte* instance, Byte* icon_name);
import Cursor*  LoadCursorA (Byte* instance, Byte* cursor_name);

import void              PostQuitMessage  (Number32 exit_code);
import Number32          PeekMessageA     (Windows_Message* message, Byte* window, Number32 filter_min, Number32 filter_max, Number32 remove_message);
import Integer_Number32  DispatchMessageA (Windows_Message* message);
import Number32          TranslateMessage (Windows_Message* message);
import Number32          GetMessageA      (Windows_Message* message, Byte* window, Number32 filter_min, Number32 filter_max);
import Number32          SendMessageA     (Byte* window, Number32 message, Number16 short_parameter, Number32 long_parameter);

import Number32          InvalidateRect   (Byte* window, Windows_Rectangle* rectangle, Number32 clean);

import Bit32 GetAsyncKeyState (Bit32 key);
import Bit32 GetCursorPos     (Windows_Point* coords);


//======== Display ====================

//https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-_display_devicea
typedef struct
{
    Bit32 structure_size;
    Bit8  device_name[32];
    Bit8  device_context_name[128];
    Bit32 state;
    Bit8  device_id[128];
    Bit8  device_key[128];
}
Dispaly;

//https://vxlab.info/wasm/article.php-article=Multimon.htm
typedef enum
{
    DISPLAY_IS_ATTACHED_TO_DESKTOP  = 0x00000001,
    DISPLAY_IS_MULTI_DRIVER         = 0x00000002,
    DISPLAY_IS_PRIMARY_DEVICE       = 0x00000004,
    DISPLAY_IS_MIRRORING_DRIVER     = 0x00000008,
    DISPLAY_IS_VGA_COMPATIBLE       = 0x00000010,
}
Display_States;

//https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-_devicemodea
typedef struct
{
    Bit8 device_name[32];
    Bit32 version;
    Bit16 structure_size;
    Bit16 private_structure_size;
    Bit32 fields;

    union {
        struct
        {
            Bit16 orientaion;
            Bit16 paper_size;
            Bit16 paper_length;
            Bit16 paper_width;
            Bit16 scale;
            Bit16 copies;
            Bit16 default_score;
            Bit16 quality;
        }
        printer;

        struct
        {
            Bit32 x;
            Bit32 y;
        }
        position;

        struct
        {
            struct
            {
                Bit32 x;
                Bit32 y;
            }
            position;

            Bit32 orientaion;
            Bit32 output;
        }
        display;
    };

    Bit16 color;
    Bit16 duplex;
    Bit16 y_resolution;
    Bit16 option;
    Bit16 collate;
    Bit8  form_name[32];
    Bit16 pixels_per_inch;
    Bit32 bits_per_pixel;
    Bit32 width;
    Bit32 height;

    union
    {
        Bit32 display_flags;
        Bit32 NUP;
    };

    Bit32 frequency;
    Bit32 ICM_method;
    Bit32 ICM_intent;
    Bit32 media_type;
    Bit32 dither_type;
    Bit32 reserved1;
    Bit32 reserved2;
    Bit32 panning_width;
    Bit32 panning_height;
}
Dispaly_Mode;

//http://www.asmcommunity.net/forums/topic/?id=2281
typedef enum
{
    DM_PELSWIDTH        = 0x00080000,
    DM_PELSHEIGHT       = 0x00100000,
    DM_DISPLAYFREQUENCY = 0x00400000
}
Display_Mode_Flags;

//https://gist.github.com/curzona/92cdeb66e850846c1e69f29b63f34691
typedef enum
{
    CDS_UPDATEREGISTRY       = 0x00000001,
    CDS_RESET                = 0x40000000
}
Change_Display_Settings_Flags;

//https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-enumdisplaydevicesa
import Bit32 EnumDisplayDevicesA (Bit8* name, Bit32 display_number, Dispaly* display, Bit32 flags);

//https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-enumdisplaysettingsa
//http://jasinskionline.com/windowsapi/ref/e/enumdisplaysettings.html
import Bit32 EnumDisplaySettingsA (Dispaly* display, Bit32 mode, Dispaly_Mode* display_mode);

//https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-changedisplaysettingsa
import Bit32 ChangeDisplaySettingsA (Dispaly_Mode* display_mode, Bit32 flags);


#endif //SYSTEM_WINDOWS_USER32_INCLUDED