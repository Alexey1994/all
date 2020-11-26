#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED


asm (
	"pusha \n"
	"call timer_interrupt_handler\n"
	"popa \n"
	"iret"
);
void timer_interrupt_handler()
{
	//tick
	out_8(0x20, 0x20);
}


void initialize_timer()
{
	out_8(0x20, 0x11);
	out_8(0xA0, 0x11);
	out_8(0x21, 0x20);
	out_8(0xA1, 40);
	out_8(0x21, 0x04);
	out_8(0xA1, 0x02);
	out_8(0x21, 0x01);
	out_8(0xA1, 0x01);
	out_8(0x21, 0x0);
	out_8(0xA1, 0x0);

	add_interrupt_handler(32, ((Number32)&timer_interrupt_handler) - 8);
}


#endif//TIMER_INCLUDED