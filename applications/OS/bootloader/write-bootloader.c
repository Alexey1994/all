#define DEBUG
#include <log/log.h>
#include <file/file.h>


typedef struct
{
    Byte active;
    N_8  head_start;
    N_8  sector_and_cilinder_high_start;
    N_8  cilinder_low_start;
    Byte type;
    N_8  head_end;
    N_8  sector_and_cilinder_high_end;
    N_8  cilinder_low_end;
    N_32 start_sector_index;
    N_32 number_of_sectors;
}
MBR;


N_32 main()
{
    File bootloader_file;
    File vm_file;
    Byte sector[512] = {0};
    MBR* mbr;

    bootloader_file = open_file(L"./bin/loader");
    
    if(!bootloader_file)
    {
        log_error("./bin/loader file not found")
        return 1;
    }

    vm_file = open_file(L"../VM/storage");

    if(!vm_file)
    {
        close_file(bootloader_file);
        log_error("../VM/storage file not found")
        return 1;
    }

    read_from_file(bootloader_file, 0, sector, 512);

    mbr = sector + 446;
    mbr[0].active                         = 0x80;
    mbr[0].head_start                     = 0x20;
    mbr[0].sector_and_cilinder_high_start = 0x21;
    mbr[0].cilinder_low_start             = 0x00;
    mbr[0].type                           = 0x0C;
    mbr[0].head_end                       = 0xFE;
    mbr[0].sector_and_cilinder_high_end   = 0xFF;
    mbr[0].cilinder_low_end               = 0xFF;
    mbr[0].start_sector_index             = 1;
    mbr[0].number_of_sectors              = 2048;

    sector[510] = 0x55;
    sector[511] = 0xAA;
    
    write_in_file(vm_file, 0, sector, 512);

    close_file(bootloader_file);
    close_file(vm_file);

    return 0;
}
