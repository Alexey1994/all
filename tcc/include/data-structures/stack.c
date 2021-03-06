#ifndef DATA_STRUCTURES_STACK_INCLUDED
#define DATA_STRUCTURES_STACK_INCLUDED


#include <system/memory.c>
#include <memory.c>


typedef struct
{
	Number size;
	Number reserve;
	Byte*  data;
}
Stack;


void initialize_stack(Stack* stack, Number reserve)
{
	stack->size    = 0;
	stack->reserve = reserve;
	stack->data    = allocate_memory(stack->reserve);
}


void deinitialize_stack(Stack* stack)
{
	free_memory(stack->data);
}


void add_bytes_in_stack(Stack* stack, Byte* bytes, Number number_of_bytes)
{
	while(stack->reserve - stack->size < number_of_bytes)
	{
		stack->reserve *= 2;
		stack->data = reallocate_memory(stack->data, stack->reserve);
	}

	copy_bytes(stack->data + stack->size, bytes, number_of_bytes);
	stack->size += number_of_bytes;
}


Number remove_bytes_from_stack(Stack* stack, Byte* bytes, Number number_of_bytes)
{
	if(number_of_bytes > stack->size)
		number_of_bytes = stack->size;

	stack->size -= number_of_bytes;
	copy_bytes(bytes, stack->data + stack->size, number_of_bytes);

	return number_of_bytes;
}


void clean_stack(Stack* stack)
{
	stack->size = 0;
}


#endif//DATA_STRUCTURES_STACK_INCLUDED

/*
void main()
{
	Stack stack;
	Byte  data[256];

	initialize_stack(&stack, 1);
	add_bytes_in_stack(&stack, "Hi", 3);
	printf("%d bytes\n", remove_bytes_from_stack(&stack, data, 256));
	printf("%s\n", data);
	deinitialize_stack(&stack);
}*/