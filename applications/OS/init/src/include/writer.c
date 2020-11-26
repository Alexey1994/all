#ifndef WRITER_INCLUDED
#define WRITER_INCLUDED


#define OUTPUT_BUFFER_SIZE 512


typedef struct
{
    Byte*    source;
    //void  (*write_byte)(Byte* source, Byte byte);
    Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 bytes_length);
    void     (*destroy_source)(Byte* source);

    Number   buffer_used;
	Byte     buffer[OUTPUT_BUFFER_SIZE];
}
Writer;


void initialize_writer(Writer* writer, Byte* source, Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 bytes_length))
{
    writer->buffer_used    = 0;
    writer->source         = source;
    writer->write_bytes    = write_bytes;
    writer->destroy_source = 0;
}


void deinitialize_writer(Writer* writer)
{
    if(writer->buffer_used)
        writer->write_bytes(writer->source, writer->buffer, writer->buffer_used);

    if(writer->destroy_source)
        writer->destroy_source(writer->source);
}


void flush_writer(Writer* writer)
{
    if(writer->buffer_used)
    {
        writer->write_bytes(writer->source, writer->buffer, writer->buffer_used);
        writer->buffer_used = 0;
    }
}


void write_byte(Writer* writer, Byte byte)
{
    writer->buffer[writer->buffer_used] = byte;
    ++writer->buffer_used;

    if(writer->buffer_used == OUTPUT_BUFFER_SIZE)
    {
        writer->write_bytes(writer->source, writer->buffer, OUTPUT_BUFFER_SIZE);
        writer->buffer_used = 0;
    }
}


void write_Number32(Writer* writer, Number32 number)
{
    Number32 number_copy;
    Number32 right_selector;

    number_copy    = number;
    right_selector = 1;

    for(;;)
    {
        number_copy /= 10;

        if(!number_copy)
            break;

        right_selector *= 10;
    }

    do
    {
        write_byte(writer, number / right_selector % 10 + '0');
        right_selector /= 10;
    }
    while(right_selector);
}


void write_byte_array(Writer* writer, Byte* data, Number32 length)
{
    Number32 i;

    for(i = 0; i < length; ++i)
        write_byte(writer, data[i]);
}


void write_byte_array_in_big_endian(Writer* writer, Byte* data, Number32 length)
{
    Bit16    a = 1;
    Number32 i;

    if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
    {
        for(i = length; i; --i)
            write_byte(writer, data[i - 1]);
    }
    else
    {
        for(i = 0; i < length; ++i)
            write_byte(writer, data[i]);
    }
}


void write_byte_array_in_little_endian(Writer* writer, Byte* data, Number32 length)
{
    Bit16    a = 1;
    Number32 i;

    if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
    {
        for(i = 0; i < length; ++i)
            write_byte(writer, data[i]);
    }
    else
    {
        for(i = length; i; --i)
            write_byte(writer, data[i - 1]);
    }
}


void write_null_terminated_byte_array(Writer* writer, Byte* data)
{
    for(; *data; ++data)
        write_byte(writer, *data);
}


void write_binary_Number32(Writer* writer, Number32 number)
{
    write_byte_array(writer, &number, 4);
}


void write_binary_Number16(Writer* writer, Number16 number)
{
    write_byte_array(writer, &number, 2);
}


void write_binary_Number8(Writer* writer, Number8 number)
{
    write_byte_array(writer, &number, 1);
}


void write_binary_Rational_Number32(Writer* writer, Rational_Number32 number)
{
    write_byte_array(writer, &number, 4);
}


void write_hex_character(Writer* writer, Byte character)
{
    if(character < 10)
        write_byte(writer, character + '0');
    else
        write_byte(writer, character - 10 + 'A');
}


void write_hex_byte(Writer* writer, Byte byte)
{
    if(byte < 16)
    {
        write_byte(writer, '0');
        write_hex_character(writer, byte);
    }
    else
    {
        write_hex_character(writer, byte >> 4);
        write_hex_character(writer, byte & 0b00001111);
    }
}


void write_hex_array(Writer* writer, Byte* array, Number32 length)
{
    Number32 i;

    for(i=0; i<length; ++i)
        write_hex_byte(writer, array[i]);
}


void write_hex_Number8(Writer* writer, Number8 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_byte_array(writer, "0x");
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number16(Writer* writer, Number16 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_byte_array(writer, "0x");
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number32(Writer* writer, Number32 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_byte_array(writer, "0x");
	write_hex_byte(writer, bytes[3]);
	write_hex_byte(writer, bytes[2]);
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


void print_in_writer(Writer* writer, ...)
{
    Byte** argument_address;
    Byte*  read_function;

    argument_address = &writer + 1;
    
    while(*argument_address)
    {
        read_function = *argument_address;

        if(read_function == &write_Number32)
        {
            write_Number32(writer, argument_address[1]);
            argument_address += 2;
        }
        else if(read_function == &write_hex_Number32)
        {
            write_hex_Number32(writer, argument_address[1]);
            argument_address += 2;
        }
        else if(read_function == &write_hex_Number16)
        {
            write_hex_Number16(writer, argument_address[1]);
            argument_address += 2;
        }
        else
        {
            write_null_terminated_byte_array(writer, argument_address[0]);
            argument_address += 1;
        }
    }
}


#define WRITER(source, write_bytes)\
{\
    Writer writer;\
    initialize_writer(&writer, (source), (write_bytes));

#define write(type, ...)\
    write_##type(&writer, ##__VA_ARGS__)

#define END_WRITER\
    deinitialize_writer(&writer);\
}


#endif//WRITER_INCLUDED