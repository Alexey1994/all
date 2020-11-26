#ifndef X_86_H_INCLUDED
#define X_86_H_INCLUDED


#include <system.h>
#include "parser.h"


#define DEV_MODE


typedef struct
{
	N_32    current_address;
	N_32    current_variable_index;
	Buffer  variables;
	Buffer* local_variables;
	Byte*   source;
	void   (*write)(Byte* source, Byte byte);
}
Generator;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 register_number;
	N_32 type2;
}
Number8_From_Stack;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 register_number;
	N_32 type2;
}
Number16_From_Stack;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 register_number;
	N_32 type2;
}
Number32_From_Stack;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Pointer_From_Stack;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Address_Getter;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Undefined_From_Stack;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Cast_To_Number8;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Cast_To_Number16;


typedef struct
{
	N_32 type;
	N_32 address;
	N_32 type2;
}
Cast_To_Number32;


#endif //X_86_H_INCLUDED