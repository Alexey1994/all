/*
Point {
	Number x
	Number y
}

0: x
4: y

push s1, 0
push s1, 0
push s2, $
ret

sum_Point {
	Point p1
	Point p2

	Point result

	result = Point(p1.x + p2.x, p1.y + p2.y)
}

0:  p1
4:  p2
8:  result

12: p1.x
16: p2.x
pop b
pop a
12: a + b
16: p1.y
20: p2.y
pop b
pop a
16: a + b
result = 16 - 8



push 0
push 0
push 0


print_Point {
	Point p

	print(p.a)
	print(", ")
	print(p.b)
}

print_Point(
	sum_Point(
		Point(1, 2),
		Point(3, 4)
	).result
)
*/

#include <system.c>
#include <memory.c>


Byte   program[256];
Number current_instruction_index;
Number data_stack[256];
Number data_stack_top;
Number code_stack[256];
Number code_stack_top;

Number base;
Number next_base;
Number a;
Number b;


#define WRITE_COMMAND(command) {program[current_instruction_index] = command; ++current_instruction_index;}
#define WRITE_NUMBER(number) {WRITE_COMMAND(number % 256); WRITE_COMMAND((number >> 8) % 256); WRITE_COMMAND((number >> 16) % 256); WRITE_COMMAND((number >> 24) % 256);}


typedef enum
{
	POP_A_FROM_DATA_COMMAND,
	POP_B_FROM_DATA_COMMAND,
	PUSH_A_IN_DATA_COMMAND,
	PUSH_B_IN_DATA_COMMAND,
	PUSH_NUMBER_IN_DATA_COMMAND,

	PUSH_A_IN_CODE_COMMAND,
	PUSH_B_IN_CODE_COMMAND,
	PUSH_NUMBER_IN_CODE_COMMAND,
	POP_A_FROM_CODE_COMMAND,
	POP_B_FROM_CODE_COMMAND,

	START_CALL_COMMAND,
	SET_ARGUMENT_TO_NUMBER_COMMAND,

	STOP_COMMAND
}
Commands;


#define PUSH_A_IN_DATA {WRITE_COMMAND(PUSH_A_IN_DATA_COMMAND);}
#define PUSH_B_IN_DATA {WRITE_COMMAND(PUSH_B_IN_DATA_COMMAND);}
#define PUSH_NUMBER_IN_DATA(number) {WRITE_COMMAND(PUSH_NUMBER_IN_DATA_COMMAND); WRITE_NUMBER(number);}

#define POP_A_FROM_DATA {WRITE_COMMAND(POP_A_FROM_DATA_COMMAND);}
#define POP_B_FROM_DATA {WRITE_COMMAND(POP_B_FROM_DATA_COMMAND);}


#define PUSH_A_IN_CODE {WRITE_COMMAND(PUSH_A_IN_CODE_COMMAND);}
#define PUSH_B_IN_CODE {WRITE_COMMAND(PUSH_B_IN_CODE_COMMAND)}
#define PUSH_NUMBER_IN_CODE(number) {WRITE_COMMAND(PUSH_NUMBER_IN_CODE_COMMAND); WRITE_NUMBER(number);}

#define POP_A_FROM_CODE {WRITE_COMMAND(POP_A_FROM_CODE_COMMAND);}
#define POP_B_FROM_CODE {WRITE_COMMAND(POP_B_FROM_CODE_COMMAND);}


//#define START_CALL_COMMAND(number_of_arguments) //{next_base = data_stack_top; Number i; for(i=0; i<number_of_arguments; ++i) push_in_data_stack(0);}
#define START_CALL(number_of_arguments) {WRITE_COMMAND(START_CALL_COMMAND); Number i; for(i=0; i<number_of_arguments; ++i) {PUSH_NUMBER_IN_DATA(0);}}
#define SET_ARGUMENT_TO_NUMBER(argument_index, number) {WRITE_COMMAND(SET_ARGUMENT_TO_NUMBER_COMMAND); WRITE_NUMBER(argument_index); WRITE_NUMBER(number);}
#define END_CALL(address) {push_in_code_stack(base); base = next_base; push_in_code_stack(current_instruction_index); current_instruction_index = address;}
#define RETURN {next_base = base; base = pop_from_code_stack(); current_instruction_index = pop_from_code_stack(); }


#define STOP {WRITE_COMMAND(STOP_COMMAND)}


void print(Number* numbers, Number number_of_numbers)
{
	Number i;
	for(i = 0; i < number_of_numbers; ++i)
		printf("%d\n", numbers[i]);
}


Byte read_byte_from_program()
{
	Byte byte;
	byte = program[current_instruction_index];
	++current_instruction_index;
	return byte;
}


Number read_number_from_program()
{
	return read_byte_from_program() +
		(read_byte_from_program() << 8) +
		(read_byte_from_program() << 16) +
		(read_byte_from_program() << 24);
}


Number pop_from_data_stack()
{
	--data_stack_top;
	return data_stack[data_stack_top];
}

void push_in_data_stack(Number data)
{
	data_stack[data_stack_top] = data;
	++data_stack_top;
}

Number pop_from_code_stack()
{
	++code_stack_top;
	return code_stack[code_stack_top];
}

void push_in_code_stack(Number data)
{
	code_stack[code_stack_top] = data;
	--code_stack_top;
}

void execute()
{
	current_instruction_index = 0;
	data_stack_top = 0;
	code_stack_top = 255;
	base = 0;
	next_base = 0;
	a = 0;
	b = 0;

	for(;;)
	{
		switch(read_byte_from_program())
		{
			case PUSH_A_IN_DATA_COMMAND:
				push_in_data_stack(a);
				break;

			case PUSH_B_IN_DATA_COMMAND:
				push_in_data_stack(b);
				break;

			case PUSH_NUMBER_IN_DATA_COMMAND:
				push_in_data_stack(read_number_from_program());
				break;

			case POP_A_FROM_DATA_COMMAND:
				a = pop_from_data_stack();
				break;

			case POP_B_FROM_DATA_COMMAND:
				b = pop_from_data_stack();
				break;

			case PUSH_A_IN_CODE_COMMAND:
				push_in_code_stack(a);
				break;

			case PUSH_B_IN_CODE_COMMAND:
				push_in_code_stack(b);
				break;

			case PUSH_NUMBER_IN_CODE_COMMAND:
				push_in_code_stack(read_number_from_program());
				break;

			case POP_A_FROM_CODE_COMMAND:
				a = pop_from_code_stack();
				break;

			case POP_B_FROM_CODE_COMMAND:
				b = pop_from_code_stack();
				break;

			case START_CALL_COMMAND:
				next_base = data_stack_top;
				break;

			case SET_ARGUMENT_TO_NUMBER_COMMAND:
				data_stack[next_base + read_number_from_program()] = read_number_from_program();
				break;

			case STOP_COMMAND:
				return;
		}
	}
}


void write_program()
{
	//Point(1, 2)
	START_CALL(2)
		SET_ARGUMENT_TO_NUMBER(0, 1)
		SET_ARGUMENT_TO_NUMBER(1, 2)
		//Number point = next_base;
		//data_stack[next_base] = 1;
		//data_stack[next_base + 1] = 2;

	//print_Point(Point(1, 2))
	START_CALL(1)
		SET_ARGUMENT_TO_NUMBER(0, 34)
		//data_stack[next_base] = point;
	
	//PUSH_A_IN_DATA
	//PUSH_NUMBER_IN_DATA(12)
	//PUSH_NUMBER_IN_DATA(123)
	STOP
}


void main()
{
	/*init();

	START_CALL
		PUSH_NUMBER_IN_DATA(1)
		PUSH_NUMBER_IN_DATA(2)
	END_CALL(0)
	
	POP_A_FROM_DATA
	printf("%d", *((Number*)data_stack + next_base));*/

	clear_bytes(program, 256);
	current_instruction_index = 0;

	write_program();

	clear_bytes(data_stack, 256);
	clear_bytes(code_stack, 256);
	execute();

	print(data_stack, 10);
}
