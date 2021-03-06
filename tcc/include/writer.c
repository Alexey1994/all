#ifndef WRITER_INCLUDED
#define WRITER_INCLUDED


#include <system.c>


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


void write_Number(Writer* writer, Number number)
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


void write_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
    Number i;

    for(i = 0; i < number_of_bytes; ++i)
        write_byte(writer, bytes[i]);
}


void write_bytes_in_big_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
{
    Bit16  a = 1;
    Number i;

    if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
    {
        for(i = number_of_bytes; i; --i)
            write_byte(writer, bytes[i - 1]);
    }
    else
    {
        for(i = 0; i < number_of_bytes; ++i)
            write_byte(writer, bytes[i]);
    }
}


void write_bytes_in_little_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
{
    Bit16  a = 1;
    Number i;

    if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
    {
        for(i = 0; i < number_of_bytes; ++i)
            write_byte(writer, bytes[i]);
    }
    else
    {
        for(i = number_of_bytes; i; --i)
            write_byte(writer, bytes[i - 1]);
    }
}


void write_null_terminated_bytes(Writer* writer, Byte* bytes)
{
    for(; *bytes; ++bytes)
        write_byte(writer, *bytes);
}


void write_binary_Number32(Writer* writer, Number32 number)
{
    write_bytes(writer, &number, 4);
}


void write_binary_Number16(Writer* writer, Number16 number)
{
    write_bytes(writer, &number, 2);
}


void write_binary_Number8(Writer* writer, Number8 number)
{
    write_bytes(writer, &number, 1);
}


void write_binary_Rational_Number32(Writer* writer, Rational_Number32 number)
{
    write_bytes(writer, &number, 4);
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


void write_hex_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
    Number i;

    for(i=0; i<number_of_bytes; ++i)
        write_hex_byte(writer, bytes[i]);
}


void write_hex_Number8(Writer* writer, Number8 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number16(Writer* writer, Number16 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number32(Writer* writer, Number32 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
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

        if(read_function == &write_Number)
        {
            write_Number(writer, argument_address[1]);
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
            write_null_terminated_bytes(writer, argument_address[0]);
            argument_address += 1;
        }
    }
}


#define WRITER(source, write_bytes_in_source)\
{\
    Writer writer;\
    initialize_writer(&writer, (source), (write_bytes_in_source));

#define write(type, ...)\
    write_##type(&writer, ##__VA_ARGS__)

#define END_WRITER\
    deinitialize_writer(&writer);\
}


#endif//WRITER_INCLUDED

/*
#define STATIC_BUFFER_LENGTH 256

typedef struct
{
    Number32 length;
    Byte data[STATIC_BUFFER_LENGTH];
}
Static_Buffer;

void initialize_static_buffer(Static_Buffer* buffer)
{
    buffer->length = 0;
}

Number write_bytes_in_buffer(Static_Buffer* buffer, Byte* bytes, Number number_of_bytes)
{
    Number i;

    for(i = 0; i < number_of_bytes; ++i)
        buffer->data[buffer->length + i] = bytes[i];    

    buffer->length += number_of_bytes;

    return number_of_bytes;
}


Number fwrite (Byte* bytes, Number byte_size, Number number_of_bytes, Byte* file);

Number write_bytes_in_file(Byte* file, Byte* bytes, Number number_of_bytes)
{
    return fwrite(bytes, 1, number_of_bytes, file);
}

void out_test()
{
    Byte* file;

    file = fopen("out_test.txt", "wb");

    WRITER(file, &write_bytes_in_file)
        write(null_terminated_bytes, "Hi! ");
        write(Number, 123);
    END_WRITER

    fclose(file);
}

void print_test()
{
    Byte* file;

    file = fopen("print_test.txt", "wb");

    WRITER(file, &write_bytes_in_file)
        print_in_writer(&writer,
            "Hi! ",
            write_Number, 123,
            " user",
        0);
    END_WRITER

    fclose(file);
}

void main ()
{
    out_test();
    print_test();
}*/