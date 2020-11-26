#include "executor.h"


static void add_variable(Parser* parser, Buffer* variable_name, N_32 variable_type_index)
{
	Variable_Operand* variable;

	variable = create_variable_operand(variable_name, parser->variables.length / sizeof(Variable_Operand*), variable_type_index);
	add_in_buffer_end(&parser->variables, Operand*, variable);
}


Variable_Operand* find_variable(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->variables, Operand*, operand)
		Variable_Operand *variable = &operand->data;

		if(!compare_UTF8_string(&variable->name, name))
			return variable;
	end_for_each_buffer_element

	return 0;
}


static void add_operand(Parser* parser, Operand* operand, Operand_Type* result_type, Byte* result)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operand->type)
	{
		case NAME_OPERAND:
		{
			Name_Operand* name_operand = &operand->data;
			
			Name_Operand* name_result = result;
			*name_result = *name_operand;
			*result_type = NAME_OPERAND;
			break;
		}

		default:
		{
			//printf("error: undefined operand type: %d\n", operand->type);
		}
	}
}


static void execute_unary_operation(
	Parser* parser,
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
			if(operand->type != NAME_OPERAND)
			{
				printf("error: variable definition\n");
				parser->has_error = 1;
				break;
			}

			Name_Operand* name_operand = &operand->data;

			if(find_variable(parser, &name_operand->name))
			{
				printf("error: variable ");
				print_token(&name_operand->name);
				printf(" was defined\n");
				parser->has_error = 1;
				break;
			}

			add_variable(parser, &name_operand->name, NUMBER);

			*result_type = CAST_TO_NUMBER32;
			break;
		}

		default:
		{
			if(((N_32)operation) & 0b10000000000000000000000000000000)
			{
				//N_32 type_index = ((N_32)operation) & 0b01111111111111111111111111111111;

				if(operand->type != NAME_OPERAND)
				{
					printf("error: variable definition\n");
					parser->has_error = 1;
					break;
				}

				Name_Operand* name_operand = &operand->data;

				if(find_variable(parser, &name_operand->name))
				{
					printf("error: variable ");
					print_token(&name_operand->name);
					printf(" was defined\n");
					parser->has_error = 1;
					break;
				}

				add_variable(parser, &name_operand->name, operation);

				*result_type = CAST_TO_NUMBER32;
			}
			else
				*result_type = UNDEFINED_FROM_STACK;
		}
	}
}


static void execute_binary_operation(
	Parser* parser,
	Byte* operation,
	Operand* left_operand,
	Operand* right_operand,
	Operand_Type* result_type, Byte* result
)
{
	*result_type = UNDEFINED_FROM_STACK;
}


static void execute_operand_sequence(Parser* parser, Buffer* operands_stack)
{

}


void calculate_variables(Parser* parser)
{
	execute(
		&parser->expressions,
		parser,
		&add_operand,
		&execute_unary_operation,
		&execute_binary_operation,
		&execute_operand_sequence,
		0
	);
}