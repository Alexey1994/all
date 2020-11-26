#ifndef X86_2_H_INCLUDED
#define X86_2_H_INCLUDED


#include <system.h>
#include <log.h>
#include "parser.h"


typedef struct
{
	Parser* parser;

	N_32    current_address;
	N_32    current_variable_index;
	Buffer  variables;
	Buffer* local_variables;
	Byte*   source;
	void   (*write_byte)(Byte* source, Byte byte);

	N_32 printf_address;
	N_32 data_address;
}
Generator;


void generate_x86(Parser* parser, Byte* source, void (*write_byte)(Byte* source, Byte byte), N_32 printf_address, N_32 data_address);


#endif //X86_2_H_INCLUDED