#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED


asm (
	"pusha \n"
	"call keyboard_interrupt_handler\n"
	"popa \n"
	"iret"
);
void keyboard_interrupt_handler()
{
	Byte pressed_key[2];
	pressed_key[0] = in_8(0x60);
	pressed_key[1] = '\0';
	print(&pressed_key, 0);
	out_8(0x20, 0x20);
}


void initialize_keyboard()
{
	add_interrupt_handler(33, ((Number32)&keyboard_interrupt_handler) - 8);
}


#endif//KEYBOARD_INCLUDED