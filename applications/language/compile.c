#define DEBUG
#include <system.h>
#include "parser.h"
#include "x86_2.h"
#include <file.h>
#include <PE.h>


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

		PE   pe;
    	N_32 pe_data_address;
    	N_32 printf_address;
    	N_32 exit_address;
		
		FILE_OUTPUT("out")
			generate_x86(&parser, &output, &write_byte, printf_address, pe_data_address);
		END_OUTPUT

		FILE_OUTPUT("out.exe")
			begin_PE(&pe, &output);
	            add_PE_library_import(&pe, "msvcrt.dll");
	            printf_address = add_PE_library_function_import(&pe, "printf");
	            add_PE_import_section(&pe);

	            BUFFER_IO()
	                write_null_terminated_byte_array(&output, "%d\n");
	                write_byte(&output, '\0');

	                write_null_terminated_byte_array(&output, "&%d\n");
	                write_byte(&output, '\0');

	                flush_output(&output);

	                pe_data_address = add_section_in_PE(
	                    &pe,
	                    "Data",
	                    INITIALIZED_DATA_PE_SECTION | ENABLE_READ_MEMORY_PE_SECTION,
	                    &input
	                );
	            END_BUFFER_IO

	            BUFFER_IO()
	            	generate_x86(&parser, &output, &write_byte, printf_address, pe_data_address);

	                flush_output(&output);
                	add_PE_code_section(&pe, &input);
	            END_BUFFER_IO
	        end_PE(&pe);
        END_OUTPUT
	}
	else
		printf("\n\nerror\n");

	printf("done\n");

	return 0;
}