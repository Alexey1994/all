#include "x86_2.h"
#include "executor.h"


static void next_address(Generator* generator)
{

}


static void out_byte(Generator* generator, Byte byte)
{
	generator->write_byte(generator->source, byte);
	++generator->current_address;
}


static void out_number8(Generator* generator, N_8 number)
{
	cycle(0, sizeof(number) * 8, 8)
		out_byte(generator, (number >> i) & 0xff);
	end
}


static void out_number32(Generator* generator, N_32 number)
{
	cycle(0, sizeof(number) * 8, 8)
		out_byte(generator, (number >> i) & 0xff);
	end
}


static void add_operand(Generator* generator, Operand* operand, Operand_Type* result_type, Byte* result)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operand->type)
	{
		case NUMBER:
		{
			Number_Operand* number_operand = &operand->data;

			//printf("PUSH %d ; Number\n", number_operand->value);
			out_byte(generator, 0x68);
			out_number32(generator, number_operand->value);

			Number_Operand* number_result = result;
			number_result->value = number_operand->value;
			*result_type = NUMBER;
			break;
		}

		case LABEL:
			break;

		case LABEL_LINK:
		{
			Label_Operand** label_operand = &operand->data;

			Expression label_expression;
			get_buffer_bytes(&generator->parser->result_expressions, (*label_operand)->expression_index * sizeof(Expression), &label_expression, sizeof(label_expression));
			N_32 label_address = label_expression.metadata;

			N_32* label_result = result;
			*label_result = label_address;
			*result_type = LABEL_ADDRESS;
			break;
		}

		case VARIABLE_LINK:
		{
			Variable_Operand** variable_operand = &operand->data;

			Variable_Operand** variable_result = result;
			*variable_result = *variable_operand;
			*result_type = VARIABLE_LINK;
			break;
		}

		default:
		{
			printf("error: undefined operand type: %d\n", operand->type);
		}
	}
}


static void execute_unary_operation(
	Generator* generator,
	Byte* operation,
	Operand* operand,
	Operand_Type* result_type, Byte* result
)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operation)
	{
		case 'N':
		{
			*result_type = CAST_TO_NUMBER32;
			break;
		}

		case '-':
		{
			switch(operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("POP EAX\n");
					out_byte(generator, 0x58);

					//printf("NEG EAX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = operand->data;
					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));
					
					//printf("NEG EAX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)operand->data);
					//printf("PUSH -%d\n", label_address);
					out_byte(generator, 0x68);
					out_number32(generator, label_address);
					break;
				}

				default:
					printf("error: undefined operand type (%d) in unary - operation\n", operand->type);
			}

			*result_type = NUMBER32_FROM_STACK;
			break;
		}

		case 'p':
		{
			switch(operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("PUSH data_address\n");
					out_byte(generator, 0x68);
					out_number32(generator, generator->data_address);

					//printf("MOV EAX, [printf_address]\n");
					out_byte(generator, 0xA1);
					out_number32(generator, generator->printf_address);

					//printf("CALL EAX\n");
					out_byte(generator, 0xFF);
					out_byte(generator, 0xD0);
					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = operand->data;

					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));
					
					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					//printf("PUSH data_address\n");
					out_byte(generator, 0x68);
					out_number32(generator, generator->data_address);

					//printf("MOV EAX, [printf_address]\n");
					out_byte(generator, 0xA1);
					out_number32(generator, generator->printf_address);

					//printf("CALL EAX\n");
					out_byte(generator, 0xFF);
					out_byte(generator, 0xD0);
					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)operand->data);

					//printf("PUSH label_address\n");
					out_byte(generator, 0x68);
					out_number32(generator, label_address);

					//printf("PUSH data_address + 4\n");
					out_byte(generator, 0x68);
					out_number32(generator, generator->data_address + 4);

					//printf("MOV EAX, [printf_address]\n");
					out_byte(generator, 0xA1);
					out_number32(generator, generator->printf_address);

					//printf("CALL EAX\n");
					out_byte(generator, 0xFF);
					out_byte(generator, 0xD0);
					break;
				}

				default:
					printf("error: undefined operand type (%d) in print operation\n", operand->type);
			}
			break;
		}

		default:
			printf("error: undefined unary operation %c\n", operation);
	}
}


static void execute_binary_operation(
	Generator* generator,
	Byte* operation,
	Operand* left_operand,
	Operand* right_operand,
	Operand_Type* result_type, Byte* result,
	N_32* expression_index
)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(right_operand->type)
	{
		case NUMBER:
		case NUMBER32_FROM_STACK:
		{
			//printf("POP EBX\n");
			out_byte(generator, 0x5B);
			
			break;
		}

		case VARIABLE_LINK:
		{
			Variable_Operand** variable_operand = right_operand->data;
			//printf("MOV EBX, [EBP - %d]\n", (*variable_operand)->variable_index);
			out_byte(generator, 0x8B);
			out_byte(generator, 0x9D);
			out_number32(generator, -(((*variable_operand)->variable_index) * 4));
			
			break;
		}

		case LABEL_ADDRESS:
		{
			N_32 label_address = *((N_32*)left_operand->data);
			//printf("MOV EBX, %d\n, label_address");
			out_byte(generator, 0xBB);
			out_number32(generator, label_address);

			break;
		}

		default:
			printf("error: undefined right operand type (%d) in binary operation\n", right_operand->type);
	}

	switch(operation)
	{
		case '=':
		{
			switch(left_operand->type)
			{
				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = left_operand->data;
					//printf("MOV [EBP - %d], EBX\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x89);
					out_byte(generator, 0x9D);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));
					break;
				}

				default:
					printf("error: undefined left operand type (%d) in = operation\n", left_operand->type);
			}

			break;
		}

		case '+':
		{
			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("POP EAX\n");
					out_byte(generator, 0x58);

					//printf("ADD EAX, EBX\n");
					out_byte(generator, 0x01);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = left_operand->data;
					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));

					//printf("ADD EAX, EBX\n");
					out_byte(generator, 0x01);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)left_operand->data);
					//printf("MOV EAX, %d\n, label_address");
					out_byte(generator, 0xB8);
					out_number32(generator, label_address);

					//printf("ADD EAX, EBX\n");
					out_byte(generator, 0x01);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				default:
					printf("error: undefined left operand type (%d) in + operation\n", left_operand->type);
			}

			break;
		}

		case '-':
		{
			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("POP EAX\n");
					out_byte(generator, 0x58);

					//printf("SUB EAX, EBX\n");
					out_byte(generator, 0x29);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = left_operand->data;
					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));

					//printf("SUB EAX, EBX\n");
					out_byte(generator, 0x29);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)left_operand->data);
					//printf("MOV EAX, %d\n, label_address");
					out_byte(generator, 0xB8);
					out_number32(generator, label_address);

					//printf("SUB EAX, EBX\n");
					out_byte(generator, 0x29);
					out_byte(generator, 0xD8);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				default:
					printf("error: undefined left operand type (%d) in - operation\n", left_operand->type);
			}

			break;
		}

		case '*':
		{
			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("POP EAX\n");
					out_byte(generator, 0x58);

					//printf("MUL EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xE3);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = left_operand->data;
					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));

					//printf("MUL EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xE3);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)left_operand->data);
					//printf("MOV EAX, %d\n, label_address");
					out_byte(generator, 0xB8);
					out_number32(generator, label_address);

					//printf("MUL EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xE3);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				default:
					printf("error: undefined left operand type (%d) in * operation\n", left_operand->type);
			}

			break;
		}

		case '/':
		{
			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					//printf("POP EAX\n");
					out_byte(generator, 0x58);

					//printf("CDQ\n");
					out_byte(generator, 0x99);

					//printf("IDIV EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xFB);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
				{
					Variable_Operand** variable_operand = left_operand->data;
					//printf("MOV EAX, [EBP - %d]\n", (*variable_operand)->variable_index);
					out_byte(generator, 0x8B);
					out_byte(generator, 0x85);
					out_number32(generator, -(((*variable_operand)->variable_index) * 4));

					//printf("CDQ\n");
					out_byte(generator, 0x99);

					//printf("IDIV EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xFB);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)left_operand->data);
					//printf("MOV EAX, %d\n", label_address");
					out_byte(generator, 0xB8);
					out_number32(generator, label_address);

					//printf("CDQ\n");
					out_byte(generator, 0x99);

					//printf("IDIV EBX\n");
					out_byte(generator, 0xF7);
					out_byte(generator, 0xFB);

					//printf("PUSH EAX\n");
					out_byte(generator, 0x50);

					*result_type = NUMBER32_FROM_STACK;
					break;
				}

				default:
					printf("error: undefined left operand type (%d) in / operation\n", left_operand->type);
			}

			break;
		}

		case 'i':
		{
			switch(left_operand->type)
			{
				case LABEL_ADDRESS:
				{
					N_32 label_address = *((N_32*)left_operand->data);

					//printf("CMP EBX, 0\n");
					out_byte(generator, 0x83);
					out_byte(generator, 0xFB);
					out_byte(generator, 0x00);

					//printf("JNE %d\n", label_address);
					out_byte(generator, 0x0F);
					out_byte(generator, 0x85);
					out_number32(generator, label_address - (generator->current_address + 4));

					break;
				}

				default:
					printf("error: undefined left operand type (%d) in if operation\n", left_operand->type);
			}
			break;
		}

		default:
			printf("error: undefined binary operation %c\n", operation);
	}
}


static void execute_operand_sequence(Generator* generator, Buffer* operands_stack, N_32* expression_index)
{
	Operand* operand = remove_operand_from_stack(operands_stack);

	switch(operand->type)
	{
		case UNDEFINED_FROM_STACK:
		case CAST_TO_NUMBER32:
			break;

		case LABEL_ADDRESS:
		{
			N_32 label_address = (*(N_32*)operand->data);
			//printf("JMP %d\n", label_address);
			out_byte(generator, 0xE9);
			out_number32(generator, label_address - (generator->current_address + 4));
			break;
		}

		default:
			printf("warn: stack not clean\n");
	}
}


static void enter_expression(Generator* generator, Expression* expression)
{
	expression->metadata = generator->current_address;
}


static void generate_function_initialize(Generator* generator)
{
	//printf("PUSH EBP\n");
	out_byte(generator, 0x55);

	//printf("MOV EBP, ESP\n");
	out_byte(generator, 0x89);
	out_byte(generator, 0xE5);

	if(generator->parser->variables.length)
	{
		//printf("SUB ESP, %d\n", generator->parser->variables.length / sizeof(Operand*) * 4);
		out_byte(generator, 0x81);
		out_byte(generator, 0xEC);
		out_number32(generator, generator->parser->variables.length / sizeof(Operand*) * 4);
	}
}


static void generate_function_deinitialize(Generator* generator)
{
	//printf("LEAVE\n");
	out_byte(generator, 0xC9);

	//printf("RET\n");
	out_byte(generator, 0xC3);
}


void generate_x86(Parser* parser, Byte* source, void (*write_byte)(Byte* source, Byte byte), N_32 printf_address, N_32 data_address)
{
	Generator generator;

	generator.parser         = parser;
	generator.printf_address = printf_address;
	generator.data_address   = data_address;

	//printf("generate x86 addresses\n");

	generator.source          = &generator;
	generator.write_byte      = &next_address;
	generator.current_address = 0;

	generate_function_initialize(&generator);
	execute(
		&parser->result_expressions,
		&generator,
		&add_operand,
		&execute_unary_operation,
		&execute_binary_operation,
		&execute_operand_sequence,
		&enter_expression
	);
	generate_function_deinitialize(&generator);

	//printf("generate x86 result code\n");

	generator.source          = source;
	generator.write_byte      = write_byte;
	generator.current_address = 0;

	generate_function_initialize(&generator);
	execute(
		&parser->result_expressions,
		&generator,
		&add_operand,
		&execute_unary_operation,
		&execute_binary_operation,
		&execute_operand_sequence,
		0
	);
	generate_function_deinitialize(&generator);
}