#define DEBUG
#include <log/log.h>
#include <file/file.h>


N_32 main()
{
    File init_file;
    File vm_file;
    Byte sector[512];
    N_32 init_file_size;

    init_file = open_file(L"./bin/init");
    if(!init_file)
    {
        log_error("./bin/init file not found")
        return 1;
    }
    init_file_size = get_file_size(init_file);

    vm_file = open_file(L"../VM/storage");
    if(!vm_file)
    {
        close_file(init_file);
        log_error("../VM/storage file not found")
        return 1;
    }

    cycle(0, init_file_size, 512)
        cycle(0, 512, 1)
            sector[i] = 0;
        end

        read_from_file(init_file, i, sector, 512);
        write_in_file(vm_file, i + 512, sector, 512);
    end

    close_file(init_file);
    close_file(vm_file);

    return 0;
}
