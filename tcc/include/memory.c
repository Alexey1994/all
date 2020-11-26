#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED


#include <system.c>


void print_bytes(Byte* bytes, Number size)
{
	Number i;
	for(i = 0; i < size; ++i)
	{
		printf("%d", bytes[i]);

		if(i != size - 1)
			printf(", ");
	}
	printf("\n");
}


void clear_bytes(Byte* bytes, Number size)
{
	Number system_size;
	Number remind_size;

	system_size = size / sizeof(Number);
	remind_size = size % sizeof(Number);

	while(system_size)
	{
		*((Number*)bytes) = 0;

		--system_size;
		bytes += sizeof(Number);
	}

	while(remind_size)
	{
		*bytes = 0;

		--remind_size;
		++bytes;
	}
}


void copy_bytes(Byte* bytes1, Byte* bytes2, Number size)
{
	Number system_size;
	Number remind;

	system_size = size / sizeof(Number);
	remind = size % sizeof(Number);

	while(system_size)
	{
		*((Number*)bytes1) = *((Number*)bytes2);

		--system_size;
		bytes1 += sizeof(Number);
		bytes2 += sizeof(Number);
	}

	while(remind)
	{
		*bytes1 = *bytes2;

		--remind;
		++bytes1;
		++bytes2;
	}
}


void swap_bytes(Byte* bytes1, Byte* bytes2, Number size)
{
	Number system_size;
	Number remind;
	Number system_temp;
	Byte   temp;

	system_size = size / sizeof(Number);
	remind = size % sizeof(Number);

	while(system_size)
	{
		system_temp = *((Number*)bytes1);
		*((Number*)bytes1) = *((Number*)bytes2);
		*((Number*)bytes2) = system_temp;

		--system_size;
		bytes1 += sizeof(Number);
		bytes2 += sizeof(Number);
	}

	while(remind)
	{
		temp = *bytes1;
		*bytes1 = *bytes2;
		*bytes2 = temp;

		--remind;
		++bytes1;
		++bytes2;
	}
}


void revert_bytes_order(Byte* bytes, Number size)
{
	Byte   temp;
	Number i;

	for(i = 0; i < size / 2; ++i)
	{
		temp = bytes[i];
		bytes[i] = bytes[size - i - 1];
		bytes[size - i - 1] = temp;
	}
}



#endif//MEMORY_INCLUDED

/*
void main()
{
	Number i;
	Byte bytes1[5] = {1, 2, 3, 4, 5};
	Byte bytes2[5] = {};

	if(0)
	{
		clear_bytes(bytes1, 5);
		print_bytes(bytes1, 5);
	}

	if(0)
	{
		copy_bytes(bytes2, bytes1, 5);
		print_bytes(bytes2, 5);
	}

	if(0)
	{
		swap_bytes(bytes1, bytes2, 5);
		print_bytes(bytes1, 5);
		print_bytes(bytes2, 5);
	}

	if(0)
	{
		revert_bytes_order(bytes1, 5);
		print_bytes(bytes1, 5);
	}
}*/