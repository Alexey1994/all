#ifndef PCI_INCLUDED
#define PCI_INCLUDED


typedef struct
{
	struct
	{
		Number8 bus;
		Number8 device;
		Number8 function_number;
	}
	ID;

	Number16 vendor;
	Number16 device;
	Number8  revision;
	Number8  class;
	Number8  subclass;

	Number32 BARs[6];
}
PCI_Device;


typedef enum
{
	PCI_VENDOR_ID           = 0,
	PCI_DEVICE_ID           = 2,
	PCI_COMMAND             = 4,
	PCI_STATUS              = 6,
	PCI_REVISION_ID         = 8,
	PCI_PROG_IF             = 9,
	PCI_SUBCLASS            = 10,
	PCI_CLASS_CODE          = 11,
	PCI_CACHE_LINE_SIZE     = 12,
	PCI_LATENCY_TIMER       = 13,
	PCI_HEADER_TYPE         = 14,
	PCI_BIST                = 15,
	PCI_BAR0                = 16,
	PCI_BAR1                = 20,
	PCI_BAR2                = 24,
	PCI_BAR3                = 28,
	PCI_BAR4                = 32,
	PCI_BAR5                  = 36,
	PCI_CARDBUS_CIS_POINTER   = 40,
	PCI_SUBSYSTEM_VENDOR_ID   = 44,
	PCI_SUBSYSTEM_ID          = 46,
	PCI_EXPANSION_ROM_ADDRESS = 48,
	PCI_CAPABILITIES_POINTER  = 52,
	PCI_INTERRUPT_LINE        = 60,
	PCI_INTERRUPT_PIN         = 61,
	PCI_MIN_GRANT             = 62,
	PCI_MAX_LATENCY           = 63
}
PCI_Field;


/*
   | bits 31-24  | bits 23-16  |  bits 15-8    | bits 7-0             |
-----------------------------------------------------------------------
00 | Device ID                 |  Vendor ID                           |
   --------------------------------------------------------------------
04 | Status                    |  Command                             |
   --------------------------------------------------------------------
08 | Class code  | Subclass    |  Prog IF      | Revision ID          |
   --------------------------------------------------------------------
0C | BIST        | Header type | Latency Timer | Cache Line Size      |
   --------------------------------------------------------------------
10 | Base address #0 (BAR0)                                           |
   --------------------------------------------------------------------
14 | Base address #1 (BAR1)                                           |
   --------------------------------------------------------------------
18 | Base address #2 (BAR2)                                           |
   --------------------------------------------------------------------
1C | Base address #3 (BAR3)                                           |
   --------------------------------------------------------------------
20 | Base address #4 (BAR4)                                           |
   --------------------------------------------------------------------
24 | Base address #5 (BAR5)                                           |
   --------------------------------------------------------------------
28 | Cardbus CIS Pointer                                              |
   --------------------------------------------------------------------
2C | Subsystem ID              | Subsystem Vendor ID                  |
   --------------------------------------------------------------------
30 | Expansion ROM base address                                       |
   --------------------------------------------------------------------
34 | Reserved                                  | Capabilities Pointer |
   --------------------------------------------------------------------
38 | Reserved                                                         |
   --------------------------------------------------------------------
3C | Max latency | Min Grant   | Interrupt PIN | Interrupt Line       |
   -------------------------------------------------------------------- 
*/


Number32 read_from_PCI_device_config(PCI_Device* device, Byte field_address)
{
	Number32 device_address;

	device_address =
		0b10000000000000000000000000000000 //enable bit
		| (device->ID.bus << 16)
		| (device->ID.device << 11)
		| (device->ID.function_number << 8)
		| field_address;

	out_32(0xCF8, device_address);
	return in_32(0xCFC);
}


void write_in_PCI_device_config(PCI_Device* device, Byte field_address, Number32 data)
{
	Number32 device_address;

	device_address =
		0b10000000000000000000000000000000 //enable bit
		| (device->ID.bus << 16)
		| (device->ID.device << 11)
		| (device->ID.function_number << 8)
		| field_address;

	out_32(0xCF8, device_address);
	out_32(0xCFC, data);
}


void print_PCI_device(PCI_Device* device)
{
	print(
		"bus:", &write_Number32, device->ID.bus,
		" device:", &write_Number32, device->ID.device,
		" PCI vendor:", &write_hex_Number16, device->vendor,
		" PCI device:", &write_hex_Number16, device->device,
		" revision ID:", &write_Number32, device->revision,
		" class:", &write_Number32, device->class,
		"\n", 0
	);
}


#include <audio/AC97.c>


void test_PCI_device(PCI_Device* device)
{
	if(device->vendor = 0x8086 && device->device == 0x2415)
		test_AC97(device);
}


void find_PCI_devices(void (on_finded)(PCI_Device* device))
{
	PCI_Device device;
	Number32   current_bus;
	Number32   device_config;

	for(current_bus = 0; current_bus < 256; ++current_bus)
	{
		for(device.ID.device = 0; device.ID.device < 32; ++device.ID.device)
		{
			for(device.ID.function_number = 0; device.ID.function_number < 8; ++device.ID.function_number)
			{
				device.ID.bus = current_bus;

				device_config = read_from_PCI_device_config(&device, 0);
				if(device_config != 0xffffffff)
				{
					device.vendor = device_config & 0xffff;
					device.device = device_config >> 16;

					device_config = read_from_PCI_device_config(&device, 0x08);
					device.class = device_config >> 24;
					device.subclass = (device_config >> 16) & 0xff;
					device.revision = device_config & 0xff;

					Number32 i;
					for(i = 0; i < 6; ++i)
						device.BARs[i] = read_from_PCI_device_config(&device, 0x10 + i * 4) & 0b11111111111111111111111111110000;

					on_finded(&device);
				}
			}
		}
	}
}


#endif//PCI_INCLUDED