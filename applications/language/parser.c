#include <memory/memory.h>

void print_token(Buffer* token);

#include "variables-calculator.h"
#include "links-calculator.h"


Z_32 compare_UTF8_string(Buffer* string1, Buffer* string2);



Label_Operand* find_label(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->labels, Operand*, operand)
		Label_Operand* label = &operand->data;

		if(!compare_UTF8_string(&label->name, name))
			return label;
	end_for_each_buffer_element

	return 0;
}


Type_Node* get_type_node(Parser* parser, N_32 type_index, Buffer* node_name)
{
	Buffer*    type;
	Type_Node* node;
	N_32       node_index;

	type = parser->types.data + type_index * sizeof(Buffer);

	for(node = type->data, node_index = 0; node_index < type->length / sizeof(Type_Node); ++node, ++node_index)
	{
		if(!compare_UTF8_string(&node->name, node_name))
			return node;
	}

	return 0;
}


N_32 get_type_node_index(Parser* parser, N_32 type_index, Buffer* node_name)
{
	Buffer*    type;
	Type_Node* node;
	N_32       node_index;

	type = parser->types.data + type_index * sizeof(Buffer);

	for(node = type->data, node_index = 0; node_index < type->length / sizeof(Type_Node); ++node, ++node_index)
	{
		if(!compare_UTF8_string(&node->name, node_name))
			return node_index;
	}

	return -1;
}


/*
Function_Body_Operand* create_function_body_operand(Buffer* expressions)
{
	Function_Body_Operand* operand;

	operand = allocate_memory(sizeof(Function_Body_Operand));
	operand->type = operand->type2 = FUNCTION_BODY;
	initialize_buffer(&operand->local_variables, 20);
	operand->expressions = expressions;

	return operand;
}
*/

Call_Operand* create_call_operand(Buffer* name)
{
	Call_Operand* operand;

	operand = allocate_memory(sizeof(Call_Operand));
	operand->type = operand->type2 = CALL;

	initialize_buffer(&operand->name, 10);
	add_buffer_in_buffer_end(&operand->name, name);

	return operand;
}


N_32 read_next_UTF_8_character_from_string(N_8** string)
{
	N_8  current_byte;
	N_32 number_of_bytes;
	N_32 result;

	current_byte = **string;
	++*string;

	if(!(current_byte & 0b10000000))
		return current_byte;

	if((current_byte & 0b11110000) == 0b11110000)
	{
		number_of_bytes = 4;
		result = (current_byte & 0b00001111) << 18;
	}
	else if((current_byte & 0b11100000) == 0b11100000)
	{
		number_of_bytes = 3;
		result = (current_byte & 0b00011111) << 12;
	}
	else if((current_byte & 0b11000000) == 0b11000000)
	{
		number_of_bytes = 2;
		result = (current_byte & 0b00111111) << 6;
	}
	else
		goto error;

	cycle(0, number_of_bytes - 1, 1)
	current_byte = **string;
	++*string;
	result |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
	end

	return result;

error:
	return 1;
}


Boolean is_latin_character(N_32 character)
{
	return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


Boolean is_cyrillic_character(N_32 character)
{
	return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}

/*
Boolean is_number_character(N_32 character)
{
	return character >= '0' && character <= '9';
}*/


N_32 read_token(Buffer* token, Input* input)
{
	N_32 character;

	clear_buffer(token);

	if(end_of_input(input))
		goto error;

	character = input_UTF_8_character(input);

	if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
		goto error;

	read_UTF_8_character(input);
	add_in_buffer_end(token, N_32, character);

	while(!end_of_input(input))
	{
		character = input_UTF_8_character(input);

		if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
			break;

		character = read_UTF_8_character(input);
		add_in_buffer_end(token, N_32, character);
	}

	return 1;

error:
	return 0;
}


Z_32 compare_token(Buffer* token, N_8* string)
{
	N_32* token_data;
	N_32  token_length;
	Z_32  result;

	token_data = token->data;
	token_length = token->length / 4;

	for(; token_length && *string; --token_length, ++token_data)
	{
		result = *token_data - read_next_UTF_8_character_from_string(&string);

		if(result)
			return result;
	}

	if(*string)
		return read_next_UTF_8_character_from_string(&string);
	else if(token_length)
		return *token_data;

	return 0;
}


Z_32 compare_UTF8_string(Buffer* string1, Buffer* string2)
{
	N_32* string1_data;
	N_32  string1_length;
	N_32* string2_data;
	N_32  string2_length;
	Z_32  result;

	string1_data = string1->data;
	string1_length = string1->length / 4;
	string2_data = string2->data;
	string2_length = string2->length / 4;

	for(; string1_length && string2_length; --string1_length, ++string1_data, --string2_length, ++string2_data)
	{
		result = *string1_data - *string2_data;

		if(result)
			return result;
	}

	if(string1_length)
		return 1;

	if(string2_length)
		return -1;

	return 0;
}


void print_token(Buffer* token)
{
	for_each_buffer_element(token, N_32, character)
		printf("%c", character);
	end_for_each_buffer_element
}


Boolean read_expression(Buffer* expression, Parser* parser);

/*
Boolean read_function_body(Parser* parser, Buffer* expressions)
{
	Buffer expression;

	initialize_buffer(expressions, 100);

	while(!end_of_input(parser->input) && input_byte(parser->input) != ']')
	{
		if(!read_expression(&expression, parser))
			goto error;

		add_in_buffer_end(expressions, Buffer, expression);
	}

	if(input_byte(parser->input) != ']')
	{
		printf("expected ]\n");
		goto error;
	}

	return 1;

error:
	return 0;
}*/

Boolean read_data_block(Parser* parser, Buffer* expressions)
{
	Expression expression;

	next_input_byte(parser->input);
	initialize_buffer(expressions, 100);

	while(!end_of_input(parser->input) && input_byte(parser->input) != ']')
	{
		if(!read_expression(&expression.expression_data, parser))
			goto error;

		add_in_buffer_end(expressions, Expression, expression);
	}

	if(input_byte(parser->input) != ']')
	{
		printf("expected ]\n");
		goto error;
	}

	next_input_byte(parser->input);

	return 1;

error:
	deinitialize_buffer(&expressions);
	return 0;
}


Byte* parse_expression_operand(Parser* parser)
{
	N_8   operand_type;
	Byte* operand;

	operand = 0;

	read_token(&parser->token, parser->input);
	read_spaces(parser->input);

	if(parser->token.length)
	{
		Boolean end_of_input_data = end_of_input(parser->input);

		if(!end_of_input_data)
		{
			N_32 character = input_UTF_8_character(parser->input);

			if(character == ':')
			{
				read_UTF_8_character(parser->input);

				if(find_label(parser, &parser->token))
				{
					printf("error: label ");
					print_token(&parser->token);
					printf("was defined\n");
				}
				else
				{
					operand = create_label_operand(&parser->token, parser->current_expression_index);
					add_in_buffer_end(&parser->labels, Label_Operand*, operand);
				}
			}
			else if(character == '(')
			{
				read_UTF_8_character(parser->input);
				read_UTF_8_character(parser->input);
				operand = create_call_operand(&parser->token);
			}
			else
			{
				operand = create_name_operand(&parser->token);
			}
		}
		else
		{
			operand = create_name_operand(&parser->token);
		}
	}
	else
	{
		operand_type = input_byte(parser->input);

		if(is_number_character(operand_type))
		{
			operand = create_number_operand(read_N_32(parser->input));
		}
		else
		{
			switch(operand_type)
			{
			/*case '[':
				next_input_byte(parser->input);

				Buffer* expressions = allocate_memory(sizeof(Buffer));
				
				if(read_function_body(parser, expressions))
				{
					next_input_byte(parser->input);
					operand = create_function_body_operand(expressions);
				}
				break;*/

			case '[':
			{
				Buffer* expressions = allocate_memory(sizeof(Buffer));
				
				if(read_data_block(parser, expressions))
					operand = create_data_block_operand(expressions);
				break;
			}

			case '"':
				next_input_byte(parser->input);

				N_32 character;
				Buffer string_data;

				initialize_buffer(&string_data, 4);

				BUFFER(string_data, 4)
				{
					while(!end_of_input(parser->input))
					{
						character = read_UTF_8_character(parser->input);

						if(character == '\\')
						{
							character = read_UTF_8_character(parser->input);
						}

						if(character == '"')
						{
							next_input_byte(parser->input);
							break;
						}

						add_in_buffer_end(&string_data, N_32, character);
					}

					operand = create_static_string_operand(&string_data);
				}
				END_BUFFER(string_data)
				break;

			default:
				printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
			}
		}
	}

	read_spaces(parser->input);

	return operand;

error:
	return 0;
}

/*
Operation operations[] = {
	{BINARY_OPERATION, "if"      },

	{BINARY_OPERATION, "&&"      },
	{BINARY_OPERATION, "||"      },

	{BINARY_OPERATION, "&"       },
	{BINARY_OPERATION, "|"       },
	{BINARY_OPERATION, "^"       },
	{BINARY_OPERATION, "<<"      },
	{BINARY_OPERATION, ">>"      },

	{BINARY_OPERATION, "=="      },
	{BINARY_OPERATION, "!="      },
	{BINARY_OPERATION, "<"       },
	{BINARY_OPERATION, "<="      },
	{BINARY_OPERATION, ">"       },
	{BINARY_OPERATION, ">="      },

	{BINARY_OPERATION, "="       },

	{BINARY_OPERATION, "*"       },
	{BINARY_OPERATION, "/"       },
	{BINARY_OPERATION, "%"       },
	{BINARY_OPERATION, "+"       },
	{BINARY_OPERATION, "-"       },
	
	{UNARY_OPERATION,  "!"       },
	{UNARY_OPERATION,  "-"       },
	{UNARY_OPERATION,  "*"       },
	{UNARY_OPERATION,  "&"       },
	{UNARY_OPERATION,  "~"       },
	
	{UNARY_OPERATION,  "Number8" },
	{UNARY_OPERATION,  "Number16"},
	{UNARY_OPERATION,  "Number32"},
	{UNARY_OPERATION,  "Number", }
};*/


Boolean read_if_operation(Input* input, Byte* operation_name)
{
	Boolean result;
	Byte    byte;

	result = 1;
	ACCUMULATED_INPUT(input)
		if(!read_if(&input, operation_name))
		{
			result = 0;
			restore_input;
		}

		byte = input_byte(&input);

		if(is_latin_character(byte) || is_number_character(byte))
		{
			result = 0;
			restore_input;
		}
		else
		{
			add_in_buffer_begin(&_target_input->buffer, Byte, byte);
		}
	END_ACCUMULATED_INPUT

	return result;
}


N_32 read_type_declaration(Parser* parser)
{
	Buffer    type;
	N_32      type_index;
	Type_Node type_node;

	initialize_buffer(&type, 10);
	type_index = parser->types.length / sizeof(Type_Node);

	next_input_byte(parser->input);
	read_spaces(parser->input);
	while(!end_of_input(parser->input) && input_byte(parser->input) != '}')
	{
		read_token(&parser->token, parser->input);
		if(!compare_token(&parser->token, "Number"))
		{
			type_node.type = NUMBER;
		}
		else
		{
			printf("error: undefined type ");
			print_token(&parser->token);
			printf("\n");
			goto error;
		}
		read_spaces(parser->input);

		initialize_buffer(&type_node.name, 10);
		read_token(&type_node.name, parser->input);
		if(!type_node.name.length)
		{
			printf("error: expected name in type field\n");
			goto error;
		}
		read_spaces(parser->input);

		add_in_buffer_end(&type, Type_Node, type_node);
	}
	
	if(end_of_input(parser->input))
		goto error;

	add_in_buffer_end(&parser->types, Buffer, type);
	next_input_byte(parser->input);

	return type_index | 0b10000000000000000000000000000000;

error:
	deinitialize_buffer(&type_node.name);
	return 0;
}


Byte* read_unary_operation(Parser* parser)
{
	Byte byte;

	byte = input_byte(parser->input);

	switch(byte)
	{
		case '-':
		case '!':
			next_input_byte(parser->input);
			return byte;

		case '{':
		{
			N_32 type_index = read_type_declaration(parser);

			if(!type_index)
			{
				printf("need break\n");
			}
			
			return type_index;
		}

		case 'p':
		{
			if(read_if_operation(parser->input, "print"))
				return 'p';

			break;
		}

		case 'N':
		{
			if(read_if_operation(parser->input, "Number"))
				return 'N';

			break;
		}

		case 'f':
		{
			if(read_if_operation(parser->input, "function"))
				return 'f';

			break;
		}
	}

	return 0;
}


Byte* read_binary_operation(Parser* parser)
{
	Byte byte;

	byte = input_byte(parser->input);

	switch(byte)
	{
		case '=':
		case '+':
		case '-':
		case '/':
		case '*':
		case '.':
			next_input_byte(parser->input);
			return byte;

		case 'i':
		{
			if(read_if_operation(parser->input, "if"))
				return 'i';
			break;
		}
	}

	return 0;
}


Boolean read_expression(Buffer* expression, Parser* parser)
{
	return parse_expression(
		expression,
		parser->input,
		parser,
		&parse_expression_operand,
		&read_unary_operation,
		&read_binary_operation
	);
}


Boolean parse(Parser* parser, Input* input)
{
	Expression expression;

	initialize_buffer(&parser->expressions, 100);
	initialize_buffer(&parser->labels, 100);
	initialize_buffer(&parser->variables, 100);
	initialize_buffer(&parser->types, 100);
	initialize_buffer(&parser->token, 20);
	initialize_buffer(&parser->result_expressions, 100);
	parser->input = input;
	parser->current_expression_index = 0;
	parser->has_error = 0;

	while(!end_of_input(input))
	{
		if(!read_expression(&expression.expression_data, parser))
			goto error;

		add_in_buffer_end(&parser->expressions, Expression, expression);
		++parser->current_expression_index;
	}

	calculate_variables(parser);
	
	if(parser->has_error)
		goto error;

	calculate_links(parser);

	if(parser->has_error)
		goto error;

	return 1;

error:
	return 0;
}