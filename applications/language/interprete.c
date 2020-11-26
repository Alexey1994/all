#define DEBUG
#include "interpreter.h"
#include <file/file.h>


N_32 main()
{
	Parser parser;
	N_32   parsed;

	FILE_INPUT("source")
		printf("parse...\n");
		parsed = parse(&parser, &input);
	END_INPUT

	if(parsed)
	{
		printf("generate...\n");
		interprete(&parser);
	}
	else
		printf("\n\nerror\n");

	printf("done\n");

	return 0;
}
