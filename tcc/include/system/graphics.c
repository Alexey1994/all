#ifndef SYSTEM_GRAPHICS_INCLUDED
#define SYSTEM_GRAPHICS_INCLUDED


#include <system.c>


typedef struct
{
	Bit32  width;
	Bit32  height;
	Bit32* data;
	Bit8*  system_graphics;
}
Graphics;


void initialize_graphics   (Graphics* graphics, Bit32 width, Bit32 height);
void draw_graphics         (Graphics* graphics);
void deinitialize_graphics (Graphics* graphics);


#ifdef __WIN32__

#include <system/Windows/gdi32.c>
#include <system/Windows/user32.c>
#include <system/Windows/opengl32.c>


static stdcall Number32 update_window_state(Byte* window, Number32 message, Number32* parameters_1, Integer_Number32* parameters_2)
{
	//Graphics* graphics = GetWindowLongA(window, USER_DATA);

	switch(message)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_DESTROY:
			return 0;

		default:
			//log(write_N_32, message)
	}

	return DefWindowProcA(window, message, parameters_1, parameters_2);
}


void initialize_graphics(Graphics* graphics, Number32 width, Number32 height)
{
	Window_Class                    window_class;
	Number32                        status;
	Windows_Pixel_Format_Descriptor pixel_format_descriptor;
	Byte*                           window;
	Byte*                           opengl_context;

	graphics->width  = width;
	graphics->height = height;

	window_class.structure_size   = sizeof(Window_Class);
	window_class.style            = UNIQUE_FOR_EACH_WINDOW_CONTEXT_CLASS_STYLE;
	window_class.window_procedure = &update_window_state;
	window_class.class            = 0;
	window_class.window           = 0;
	window_class.instance         = 0;
	window_class.icon             = LoadIconA(0, DEFAULT_ICON);
	window_class.cursor           = LoadCursorA(0, DEFAULT_ARROW);
	window_class.background       = 0;
	window_class.menu_name        = 0;
	window_class.class_name       = "Main window class";
	window_class.small_icon       = LoadIconA(0, DEFAULT_ICON);

	status = RegisterClassExA(&window_class);

	if(!status)
		goto error;

	window = CreateWindowExA(
		0,
		"Main window class",
		"Main",
		WS_VISIBLE,
		640,
		0,
		width,
		height,
		0,
		0,
		window_class.instance,
		0
	);

	if(!window)
	{
		//log_error("CreateWindowExA")
		goto error;
	}

	//SetWindowLongA(window, USER_DATA, graphics);
	ShowWindow(window, SHOW_MAXIMIZED_WINDOW);

	graphics->system_graphics = GetDC(window);

	pixel_format_descriptor.size            = sizeof(Windows_Pixel_Format_Descriptor);
	pixel_format_descriptor.nVersion        = 1;
	pixel_format_descriptor.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixel_format_descriptor.iPixelType      = PFD_TYPE_RGBA;
	pixel_format_descriptor.cColorBits      = 24;
	pixel_format_descriptor.cRedBits        = 0;
	pixel_format_descriptor.cRedShift       = 0;
	pixel_format_descriptor.cGreenBits      = 0;
	pixel_format_descriptor.cGreenShift     = 0;
	pixel_format_descriptor.cBlueBits       = 0;
	pixel_format_descriptor.cBlueShift      = 0;
	pixel_format_descriptor.cAlphaBits      = 0;
	pixel_format_descriptor.cAlphaShift     = 0;
	pixel_format_descriptor.cAccumBits      = 0;
	pixel_format_descriptor.cAccumRedBits   = 0;
	pixel_format_descriptor.cAccumGreenBits = 0;
	pixel_format_descriptor.cAccumBlueBits  = 0;
	pixel_format_descriptor.cAccumAlphaBits = 0;
	pixel_format_descriptor.cDepthBits      = 16;
	pixel_format_descriptor.cStencilBits    = 0;
	pixel_format_descriptor.cAuxBuffers     = 0;
	pixel_format_descriptor.iLayerType      = PFD_MAIN_PLANE;
	pixel_format_descriptor.bReserved       = 0;
	pixel_format_descriptor.dwLayerMask     = 0;
	pixel_format_descriptor.dwVisibleMask   = 0;
	pixel_format_descriptor.dwDamageMask    = 0;

	if(!SetPixelFormat(graphics->system_graphics, ChoosePixelFormat(graphics->system_graphics, &pixel_format_descriptor), &pixel_format_descriptor))
	{
		//log_error("SetPixelFormat");
		goto error;
	}

	opengl_context = wglCreateContext(graphics->system_graphics);
	if(!opengl_context)
	{
		//log_error("wglCreateContext");
		goto error;
	}

	if(!wglMakeCurrent(graphics->system_graphics, opengl_context))
	{
		//log_error("wglMakeCurrent");
		goto error;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;

error:
}


void draw_graphics(Graphics* graphics)
{
	Windows_Message message;

	if(PeekMessageA(&message, 0, 0, 0, REMOVED_FROM_QUEUE_MESSAGE))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
		/*
		if(message.message == QUIT_WINDOW_MESSAGE)
			;//break;
		else
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}*/
	}

	SwapBuffers(graphics->system_graphics);
}

#endif


#endif//SYSTEM_GRAPHICS_INCLUDED


void test_2D()
{
	Graphics graphics;
	Number32* canvas = malloc(1440 * 900 * 4);

	initialize_graphics(&graphics, 1440, 900);

	for(;;)
	{
		Number i;
		for(i = 0; i < graphics.width * graphics.height; ++i)
			canvas[i] = (128 << 24) + 128;

		glClear(GL_COLOR_BUFFER_BIT);
		glRasterPos2f(-1, -1);
		glDrawPixels(1440, 900, GL_RGBA, GL_UNSIGNED_BYTE, canvas);

		draw_graphics(&graphics);
	}
}


void test_3D()
{
	Graphics graphics;

	initialize_graphics(&graphics, 1440, 900);

	for(;;)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 1, 0);
		glEnd();

		draw_graphics(&graphics);
	}
}


void main()
{
	test_2D();
	//test_3D();
}