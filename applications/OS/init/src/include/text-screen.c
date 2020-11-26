#ifndef TEXT_SCREEN_INCLUDED
#define TEXT_SCREEN_INCLUDED


#include <writer.c>


typedef struct
{
	Bit16*   address;
	Number32 x_coord;
	Number32 y_coord;
	Number32 width;
	Number32 height;
}
Text_Screen;


void initialize_text_screen(Text_Screen* text_screen, Byte* address, Number32 width, Number32 height)
{
	text_screen->address = address;
	text_screen->x_coord = 0;
	text_screen->y_coord = 0;
	text_screen->width   = width;
	text_screen->height  = height;
}


Text_Screen* get_default_text_screen()
{
	return 0x7C00;
}


Writer* get_default_text_screen_writer()
{
	return 0x7C00 + sizeof(Text_Screen);
}


void write_byte_in_text_screen(Text_Screen* screen, Byte byte)
{
	if(byte == '\n')
	{
		++screen->y_coord;
		screen->x_coord = 0;
	}
	else
	{
		screen->address[screen->width * screen->y_coord + screen->x_coord] = byte + (15 << 8);
		++screen->x_coord;
	}

	if(screen->x_coord == screen->width)
	{
		screen->x_coord = 0;
		++screen->y_coord;
	}

	if(screen->y_coord == screen->height)
	{
		Number i;

		//shift up
		for(i = 0; i < screen->width * (screen->height - 1); ++i)
			screen->address[i] = screen->address[i + screen->width];

		//clear last line
		for(i=screen->width * (screen->height - 1); i < screen->width * screen->height; ++i)
			screen->address[i] = 0;

		--screen->y_coord;
	}
}


Number32 write_bytes_in_text_screen(Text_Screen* screen, Byte* bytes, Number32 bytes_length)
{
	Number i;
	for(i = 0; i < bytes_length; ++i)
		write_byte_in_text_screen(screen, bytes[i]);

	return bytes_length;
}


void initialize_default_text_screen()
{
	Text_Screen* default_text_screen;
	Writer*      default_text_screen_writer;

	default_text_screen = get_default_text_screen();
	default_text_screen_writer = get_default_text_screen_writer();
	initialize_text_screen(default_text_screen, 0xB8000, 80, 25);
	initialize_writer(default_text_screen_writer, default_text_screen, &write_bytes_in_text_screen);
}


Byte* _Number32 = &write_Number32;


void print(Byte* first_argument, ...)
{
	Writer* writer;
	Byte**  argument_address;
	Byte*   read_function;

	writer = get_default_text_screen_writer();
	argument_address = &first_argument;

	while(*argument_address)
	{
		read_function = *argument_address;

		if(read_function == _Number32)
		{
			write_Number32(writer, argument_address[1]);
			argument_address += 2;
		}
		else if(read_function == &write_hex_Number32)
		{
			write_hex_Number32(writer, argument_address[1]);
			argument_address += 2;
		}
		else if(read_function == &write_hex_Number16)
		{
			write_hex_Number16(writer, argument_address[1]);
			argument_address += 2;
		}
		else
		{
			write_null_terminated_byte_array(writer, argument_address[0]);
			argument_address += 1;
		}
	}

	flush_writer(writer);
}


#endif//TEXT_SCREEN_INCLUDED