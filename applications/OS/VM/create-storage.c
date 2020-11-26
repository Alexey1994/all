#include <file/file.h>


N_32 main()
{
	Byte zeros[512] = {0};
	File storage_file;

	create_file(L"storage", 0, 0);
	storage_file = open_file(L"storage");
	cycle(0, 4096, 1)
		write_in_file(storage_file, i * 512, zeros, 512);
	end
	close_file(storage_file);

	return 0;
}