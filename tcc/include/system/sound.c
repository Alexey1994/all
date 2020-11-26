#ifndef SYSTEM_SOUND_INCLUDED
#define SYSTEM_SOUND_INCLUDED


#include <system.c>


Boolean initialize_sound_device(
	Byte* source,
	Number (*read_bytes_from_source)(
		Byte* source,
		Number16* sound_buffer,
		Number size_of_sound_buffer
	)
);


#ifdef __WIN32__

#include <system/Windows/winmm.c>
#include <memory.c>


#define SOUND_BUFFER_LENGTH 8820 // in bytes

/*
	MMSYSERR_NOERROR    = 0,
	MMSYSERR_ERROR      = 1,
	MMSYSERR_BADDEVICEID    = 2,
	MMSYSERR_NOTENABLED     = 3,
	MMSYSERR_ALLOCATED      = 4,
	MMSYSERR_INVALHANDLE    = 5,
	MMSYSERR_NODRIVER       = 6,
	MMSYSERR_NOMEM      = 7,
	MMSYSERR_NOTSUPPORTED   = 8,
	MMSYSERR_BADERRNUM      = 9,
	MMSYSERR_INVALFLAG      = 10,
	MMSYSERR_INVALPARAM     = 11,
	MMSYSERR_HANDLEBUSY     = 12,
	MMSYSERR_INVALIDALIAS   = 13,
	MMSYSERR_BADDB      = 14,
	MMSYSERR_KEYNOTFOUND    = 15,
	MMSYSERR_READERROR      = 16,
	MMSYSERR_WRITEERROR     = 17,
	MMSYSERR_DELETEERROR    = 18,
	MMSYSERR_VALNOTFOUND    = 19,
	MMSYSERR_NODRIVERCB     = 20,
	WAVERR_BADFORMAT    = 32,
	WAVERR_STILLPLAYING     = 33,
	WAVERR_UNPREPARED       = 34
*/

typedef enum
{
	waveOutPrepareHeader_Error_ERROR          = 1,
	waveOutPrepareHeader_Error_BADDEVICEID    = 2,
	waveOutPrepareHeader_Error_NOTENABLED     = 3,
	waveOutPrepareHeader_Error_ALLOCATED      = 4,
	waveOutPrepareHeader_Error_INVALHANDLE    = 5,
	waveOutPrepareHeader_Error_NODRIVER       = 6,
	waveOutPrepareHeader_Error_NOMEM          = 7,
	waveOutPrepareHeader_Error_NOTSUPPORTED   = 8,
	waveOutPrepareHeader_Error_BADERRNUM      = 9,
	waveOutPrepareHeader_Error_INVALFLAG      = 10,
	waveOutPrepareHeader_Error_INVALPARAM     = 11,
	waveOutPrepareHeader_Error_HANDLEBUSY     = 12,
	waveOutPrepareHeader_Error_INVALIDALIAS   = 13,
	waveOutPrepareHeader_Error_BADDB          = 14,
	waveOutPrepareHeader_Error_KEYNOTFOUND    = 15,
	waveOutPrepareHeader_Error_READERROR      = 16,
	waveOutPrepareHeader_Error_WRITEERROR     = 17,
	waveOutPrepareHeader_Error_DELETEERROR    = 18,
	waveOutPrepareHeader_Error_VALNOTFOUND    = 19,
	waveOutPrepareHeader_Error_NODRIVERCB     = 20,

	waveOutPrepareHeader_Error_BADFORMAT        = 32,
	waveOutPrepareHeader_Error_STILLPLAYING     = 33,
	waveOutPrepareHeader_Error_UNPREPARED       = 34
}
waveOutPrepareHeader_Errors;


typedef struct
{
	Windows_Sound_Buffer  sound_buffer;
	Byte                  buffer1_data[SOUND_BUFFER_LENGTH];
	Byte                  buffer2_data[SOUND_BUFFER_LENGTH];
	Windows_Sound_Buffer  buffer1;
	Windows_Sound_Buffer  buffer2;
	Number                current_buffer_index;

	Byte*    source;
	Number (*read_bytes_from_source)(
		Byte* source,
		Number16* sound_buffer,
		Number size_of_sound_buffer
	);
}
Sound_Data;


static stdcall void play(Byte* sound_device, Number32 message, Sound_Data* data, Number32* parameter1, Number32* parameter2)
{
	if(message == MM_WOM_DONE)
	{
		if(data->current_buffer_index)
		{
			clear_bytes(data->buffer1_data, data->buffer1.buffer_length);
			data->read_bytes_from_source(data->source, data->buffer1_data, data->buffer1.buffer_length);
			waveOutWrite(sound_device, &data->buffer1, sizeof(Windows_Sound_Buffer));
		}
		else
		{
			clear_bytes(data->buffer2_data, data->buffer2.buffer_length);
			data->read_bytes_from_source(data->source, data->buffer2_data, data->buffer2.buffer_length);
			waveOutWrite(sound_device, &data->buffer2, sizeof(Windows_Sound_Buffer));
		}

		data->current_buffer_index = !data->current_buffer_index;
	}
}


static Byte*      sound_device = 0;
static Sound_Data data         = {0};


Boolean initialize_sound_device(
	Byte* source,
	Number (*read_bytes_from_source)(
		Byte* source,
		Number16* sound_buffer,
		Number size_of_sound_buffer
	)
)
{
	Number32              status;
	Windows_Sound_Format  sound_format;

	data.source                 = source;
	data.read_bytes_from_source = read_bytes_from_source;
	data.current_buffer_index   = 1;

	data.buffer1.data           = data.buffer1_data;
	data.buffer1.buffer_length  = SOUND_BUFFER_LENGTH;
	data.buffer1.bytes_recorded = 0;
	data.buffer1.flags          = 0;
	data.buffer1.loops          = 0;

	data.buffer2.data           = data.buffer2_data;
	data.buffer2.buffer_length  = SOUND_BUFFER_LENGTH;
	data.buffer2.bytes_recorded = 0;
	data.buffer2.flags          = 0;
	data.buffer2.loops          = 0;

	sound_format.format                       = PCM_WAVE_FORMAT;
	sound_format.number_of_channels           = 2;
	sound_format.samples_per_seconds          = 44100;
	sound_format.bits_per_sample              = 16;
	sound_format.block_align                  = sound_format.bits_per_sample / 8 * sound_format.number_of_channels;
	sound_format.bytes_per_seconds            = sound_format.samples_per_seconds * sound_format.block_align;
	sound_format.size_of_appended_information = 0;

	status = waveOutOpen(&sound_device, -1, &sound_format, &play, &data, CALLBACK_FUNCTION);

	if(status)
	{
		switch(status)
		{
			default:
			printf("unclassified");
		}

		printf(" error in waveOutOpen\n");

		goto error;
	}

	status = waveOutPrepareHeader(sound_device, &data.buffer1, sizeof(Windows_Sound_Buffer));

	if(status)
	{
		switch(status)
		{
			case waveOutPrepareHeader_Error_INVALHANDLE:
				printf("invalid handle");
				break;

			default:
				printf("unclassified");
		}

		printf(" error in waveOutPrepareHeader\n");

		goto error;
	}

	status = waveOutPrepareHeader(sound_device, &data.buffer2, sizeof(Windows_Sound_Buffer));

	if(status)
	{
		switch(status)
		{
			case waveOutPrepareHeader_Error_INVALHANDLE:
			printf("invalid handle");
			break;

			default:
			printf("unclassified");
		}

		printf(" error in waveOutPrepareHeader\n");

		goto error;
	}

	clear_bytes(data.buffer1_data, data.buffer1.buffer_length);
	read_bytes_from_source(source, data.buffer1_data, SOUND_BUFFER_LENGTH);
	waveOutWrite(sound_device, &data.buffer1, sizeof(Windows_Sound_Buffer));

	clear_bytes(data.buffer2_data, data.buffer2.buffer_length);
	read_bytes_from_source(source, data.buffer2_data, SOUND_BUFFER_LENGTH);
	waveOutWrite(sound_device, &data.buffer2, sizeof(Windows_Sound_Buffer));

	return 1;

error:
	return 0;
}

#endif//__WIN32__


#endif//SYSTEM_SOUND_INCLUDED

/*
Number32 sound_data[SOUND_BUFFER_LENGTH / 2];


Number read_sound_data(Number16* data, Number16* sound_buffer, Number size_of_sound_buffer)
{
	copy_bytes(sound_buffer, data, size_of_sound_buffer);
	return size_of_sound_buffer;
}


void main()
{
	Number i;
	for(i = 0; i < SOUND_BUFFER_LENGTH / 2; ++i)
		((Number32*)((Byte*)sound_data + 2))[i] = i % 200 + 190;

	initialize_sound_device(sound_data, &read_sound_data);

	for(;;);
}*/