#ifndef UTF_8_INCLUDED
#define UTF_8_INCLUDED

/*
typedef Number16 UTF_8_Character;


Integer_Number compare_UTF_8_string_and_null_terminated_string(UTF_8_Character* utf_8_string, Number8* null_terminated_string)
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
}*/


#include <system.c>


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


Integer_Number compare_UTF_8_strings(Number16* string1, Number16* string2)
{
	Integer_Number character1;
	Integer_Number character2;
	Integer_Number result;

	result = 0;

	for(;;)
	{
		character1 = read_next_UTF_8_character_from_string(&string1);
		character2 = read_next_UTF_8_character_from_string(&string2);
		result = character1 - character2;

		if(result)
			return result;

		if(!(character1 && character2))
			return result;
	}
}


#endif//UTF_8_INCLUDED


void main()
{
	printf("%d", compare_UTF_8_strings("b", "a"));
}