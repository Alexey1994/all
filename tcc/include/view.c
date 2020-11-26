#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED


#include <log.c>
#include <system/Windows/kernel32.c>
#include <system/Windows/user32.c>
#include <system/Windows/gdi32.c>


//https://github.com/AHK-just-me/AHK_Gui_Constants/tree/master/Sources


typedef struct
{
	Byte*            window;

	Integer_Number32 x;
	Integer_Number32 y;
	Integer_Number32 width;
	Integer_Number32 height;

	Bit16*           name;

	void (*message_handler)(
		struct View* view,
		Number message,
		Number parameter1,
		Number parameter2
	);
}
View;


static stdcall Number32 update_window_state(Byte* window, Number32 message, Number32* parameter1, Integer_Number32* parameter2)
{
	View* view;

	view = GetWindowLongW(window, USER_DATA);
	if(view && view->message_handler)
		view->message_handler(view, message, parameter1, parameter2);

	return DefWindowProcA(window, message, parameter1, parameter2);
}


Boolean initialize_view(
	View* view,
	void (*message_handler)(
		View* view,
		Byte* target,
		Number message,
		Number parameter1,
		Number parameter2
	)
)
{
	Window_Class window_class;
	Bit32        status;

	view->message_handler = message_handler;

	window_class.structure_size   = sizeof(Window_Class);
	window_class.style            = 0;//UNIQUE_FOR_EACH_WINDOW_CONTEXT_CLASS_STYLE;
	window_class.window_procedure = &update_window_state;
	window_class.class            = 0;
	window_class.window           = 0;
	window_class.instance         = 0;
	window_class.icon             = 0;//LoadIconA(0, DEFAULT_ICON);
	window_class.cursor           = LoadCursorA(0, DEFAULT_ARROW);
	window_class.background       = COLOR_BACKGROUND;//0;
	window_class.menu_name        = 0;
	window_class.class_name       = view->name;
	window_class.small_icon       = LoadIconA(0, DEFAULT_ICON);

	status = RegisterClassExW(&window_class);

	if(!status)
		goto error;

	view->window = CreateWindowExW(
		0,
		view->name,
		view->name,
		WS_OVERLAPPED,
		view->x,
		view->y,
		view->width,
		view->height,
		0,
		0,
		0,//window_class.instance,
		0
	);

	if(!view->window)
	{
		log_error("CreateWindowExW")
		goto error;
	}

	SetWindowLongW(view->window, USER_DATA, view);
	ShowWindow(view->window, SHOW_DEFAULT_WINDOW);

	return 1;

error:
	return 0;
}


typedef enum
{
	ALIGN_LEFT   = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT  = 2
}
Text_Align;


typedef struct
{
	Byte*            window;

	Integer_Number32 x;
	Integer_Number32 y;
	Integer_Number32 width;
	Integer_Number32 height;

	Bit16*           text;

	Text_Align       align;
}
Text;


Boolean create_text(View* view, Text* text)
{
	Number32 align;

	switch(text->align)
	{
		case ALIGN_CENTER:
			align = SS_CENTER;
			break;

		case ALIGN_RIGHT:
			align = SS_RIGHT;
			break;

		default:
			align = SS_LEFT;
	}

	text->window = CreateWindowExW(
		0,
		L"STATIC",
		text->text,
		WS_VISIBLE | WS_CHILD | align,
		text->x,
		text->y,
		text->width,
		text->height,
		view->window,
		0,
		GetWindowLongA(view->window, GWL_HINSTANCE),
		0
	);

	if(!text->window)
	{
		log_error("create_text")
		goto error;
	}

	return 1;

error:
	return 0;
}


void update_text(Text* text)
{
	Number32 align;

	switch(text->align)
	{
		case ALIGN_CENTER:
			align = SS_CENTER;
			break;

		case ALIGN_RIGHT:
			align = SS_RIGHT;
			break;

		default:
			align = SS_LEFT;
	}
	SetWindowLongW(text->window, GWL_STYLE, WS_VISIBLE | WS_CHILD | align);
}


typedef struct
{
	Byte*            window;

	Integer_Number32 x;
	Integer_Number32 y;
	Integer_Number32 width;
	Integer_Number32 height;

	Bit16*           text;
}
Button;

Boolean create_button(View* view, Button* button)
{
	button->window = CreateWindowExW(
		0,
		L"BUTTON",
		button->text,
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		button->x,
		button->y,
		button->width,
		button->height,
		view->window,
		0,
		GetWindowLongA(view->window, GWL_HINSTANCE),
		0
	);

	if(!button->window)
	{
		log_error("create_button")
		goto error;
	}

	return 1;

error:
	return 0;
}


void update_button(Button* button)
{
	SendMessageW(button->window, WM_SETTEXT, 0, button->text);
}


typedef struct
{
	Byte*            window;

	Integer_Number32 x;
	Integer_Number32 y;
	Integer_Number32 width;
	Integer_Number32 height;

	Bit16*           text;
}
Edit_Text;


Boolean create_edit_text(View* view, Edit_Text* edit_text)
{
	edit_text->window = CreateWindowExW(
		0,
		L"EDIT",
		edit_text->text,
		WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,
		edit_text->x,
		edit_text->y,
		edit_text->width,
		edit_text->height,
		view->window,
		0,
		GetWindowLongA(view->window, GWL_HINSTANCE),
		0
	);

	if(!edit_text->window)
	{
		log_error("create_edit_text")
		goto error;
	}

	return 1;

error:
	return 0;
}


void update_edit_text(Edit_Text* edit_text)
{
	SendMessageW(edit_text->window, WM_SETTEXT, 0, edit_text->text);
}



void draw_views()
{
	Windows_Message message;

	while(GetMessageA(&message, 0, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}


#endif//VIEW_H_INCLUDED

/*
View view_1 = {
	.x = 0,
	.y = 0,
	.width = 300,
	.height = 300,
	.name = L"View 1"
};

Text text_1_1 = {
	.x = 0,
	.y = 64,
	.width = 100,
	.height = 100,
	.text = L"dkfjdaj ijiojadiojgioadiogjaio jgiojiojg iojs ioj giosdj"
};

Button button_1_1 = {
	.x = 0,
	.y = 0,
	.width = 100,
	.height = 32,
	.text = L"But ton"
};

Edit_Text edit_text_1_1 = {
	.x = 0,
	.y = 184,
	.width = 100,
	.height = 32,
	.text = L"Type text"
};

void view_1_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	switch(message)
	{
		case WM_COMMAND:
			if(parameter2 == button_1_1.window)
			{
				printf("button 1 click\n");
			}
			break;
	}
}


View view_2 = {
	.x = 0,
	.y = 320,
	.width = 300,
	.height = 300,
	.name = L"View 2"
};

Button button_2_1 = {
	.x = 0,
	.y = 0,
	.width = 100,
	.height = 32,
	.text = L"Exit"
};

void view_2_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	switch(message)
	{
		case WM_COMMAND:
			if(parameter2 == button_2_1.window)
				PostQuitMessage(0);
			break;
	}
}


void main()
{
	//print_logical_drives();

	initialize_view(&view_1, &view_1_message_handler);

	create_text(&view_1, &text_1_1);
	create_button(&view_1, &button_1_1);
	create_edit_text(&view_1, &edit_text_1_1);

	initialize_view(&view_2, &view_2_message_handler);
	create_button(&view_2, &button_2_1);

	draw_views();
}*/