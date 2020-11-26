void _start();
void main()
{
	_start();
}


#include <system.c>
#include <writer.c>
#include <text-screen.c>
#include <IO.c>
#include <interrupts.c>
#include <errors.c>
#include <keyboard.c>
#include <timer.c>
#include <PCI.c>


void _start()
{
	initialize_default_text_screen();
	initialize_timer();
	initialize_keyboard();

	//print("Hi ", &write_Number32, 1234, 0);
	//find_PCI_devices(&print_PCI_device);
	find_PCI_devices(&test_PCI_device);
}