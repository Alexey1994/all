#ifndef INTERRUPTS_INCLUDED
#define INTERRUPTS_INCLUDED


typedef enum
{
	TASK_GATE_32      = 0b0101,
	INTERRUPT_GATE_16 = 0b0110,
	TRAP_GATE_16      = 0b0111,
	INTERRUPT_GATE_32 = 0b1110,
	TRAP_GATE_32      = 0b1111
}
Interrupt_Type;


#pragma pack(push, 1)
typedef struct
{
	Number16 handler_address_low;
	Number16 selector;
	Number8  zero;
	Number8  attributes;
	Number16 handler_address_high;
} __attribute__((packed))
IDT_Node;
#pragma pack(pop)


void add_interrupt_handler(Number8 interrupt_number, Bit32 handler)
{
	IDT_Node* node = 0;
	asm("cli");
	node[interrupt_number].handler_address_low = handler;
	node[interrupt_number].handler_address_high = handler >> 16;
	node[interrupt_number].selector = 8;
	node[interrupt_number].zero = 0;
	node[interrupt_number].attributes = 0b10000000 | INTERRUPT_GATE_32;
	asm("sti");
}


#endif//INTERRUPTS_INCLUDED