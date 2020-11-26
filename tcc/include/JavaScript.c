#ifndef JAVASCRIPT_INCLUDED
#define JAVASCRIPT_INCLUDED


#include <reader.c>
#include <expression-parser.c>
#include <data-structures/stack.c>


typedef struct
{
	Stack token;
	Reader* reader;
}
JavaScript_Parser;


void initialize_JavaScript_parser(JavaScript_Parser* parser)
{
	initialize_stack(&parser->token, 128);
}


void deinitialize_JavaScript_parser(JavaScript_Parser* parser)
{
	deinitialize_stack(&parser->token);
}


Boolean is_latin_character(Number16 character)
{
	return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


Boolean is_cyrillic_character(Number16 character)
{
	return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}


static Boolean parse_JavaScript_token(JavaScript_Parser* parser)
{
	Number16 character;

	clean_stack(&parser->token);

	if(end_of_reader(parser->reader))
		goto error;

	character = get_reader_UTF_8_character(parser->reader);

	if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
		goto error;

	read_UTF_8_character(parser->reader);
	add_bytes_in_stack(&parser->token, &character, sizeof(Number16));

	while(!end_of_reader(parser->reader))
	{
		character = get_reader_UTF_8_character(parser->reader);

		if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
			break;

		character = read_UTF_8_character(parser->reader);
		add_bytes_in_stack(&parser->token, &character, sizeof(Number16));
	}

	return 1;

error:
	return 0;
}


Number32 read_next_UTF_8_character_from_string(Number8** string)
{
	Number8  current_byte;
	Number   number_of_bytes;
	Number32 result;
	Number   i;

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

	for(i = 0; i < number_of_bytes - 1; ++i)
	{
		current_byte = **string;
		++*string;
		result |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
	}

	return result;

error:
	return 1;
}


Integer_Number compare_token(Stack* token, Number8* string)
{
	Number16*      token_data;
	Number         token_size;
	Integer_Number result;

	token_data = token->data;
	token_size = token->size / 4;

	for(; token_size && *string; --token_size, ++token_data)
	{
		result = *token_data - read_next_UTF_8_character_from_string(&string);

		if(result)
			return result;
	}

	if(*string)
		return read_next_UTF_8_character_from_string(&string);
	else if(token_size)
		return *token_data;

	return 0;
}


void print_token(Stack* token)
{
	Number    size;
	Number16* data;
	Number    i;

	size = token->size;
	data = token->data;
	for(i = 0; i < size; ++i)
		printf("%c", data[i]);
}


Integer_Number compare_UTF8_string(Buffer* string1, Buffer* string2)
{
	Number16*      string1_data;
	Number         string1_length;
	Number16*      string2_data;
	Number         string2_length;
	Integer_Number result;

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


static Byte* parse_JavaScript_operand(JavaScript_Parser* parser)
{
	return 0;
}


typedef enum
{
	JS_UNARY_PLUS,
	JS_UNARY_MINUS,
}
Unary_JavaScript_Operation;

static Operation unary_JavaScript_operations[] = {
	{JS_UNARY_PLUS,  1},
	{JS_UNARY_MINUS, 1}
};

static Byte* parse_unary_JavaScript_operand(JavaScript_Parser* parser)
{
	return 0;
}


typedef enum
{
	JS_PLUS,
	JS_MINUS,
	JS_MULTIPLY,
	JS_DIVIDE
}
Binary_JavaScript_Operation;

static Operation binary_JavaScript_operations[] = {
	{JS_PLUS,     2},
	{JS_MINUS,    2},
	{JS_MULTIPLY, 1},
	{JS_DIVIDE,   1},
};

static Byte* parse_binary_JavaScript_operand(JavaScript_Parser* parser)
{
	return 0;
}


static Boolean parse_JavaScript_expression(Stack* expression, JavaScript_Parser* parser)
{
	return parse_expression(
		expression,
		parser->reader,
		parser,

		&parse_JavaScript_operand,
		&parse_unary_JavaScript_operand,
		&parse_binary_JavaScript_operand
	);
}


void parse_JavaScript(JavaScript_Parser* parser, Reader* reader)
{
	Stack expression;

	parser->reader = reader;
	parse_JavaScript_expression(&expression, parser);
}


#endif//JAVASCRIPT_INCLUDED


void main()
{
	JavaScript_Parser parser;
	Reader reader;

	initialize_JavaScript_parser(&parser);
	initialize_null_terminated_bytes_reader(&reader,
		"var a = 1"
	);
	parse_JavaScript(&parser, &reader);
	deinitialize_JavaScript_parser(&parser);
}