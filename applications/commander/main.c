#include <view.c>


void print_logical_drives()
{/*
	N_32 drives = GetLogicalDrives();

	cycle(0, sizeof(Number32) * 8, 1)
		if((drives >> i) & 1)
		{
			Byte buf[256];
			snprintf(buf, 256, "%c:\\", i + 'A');

			Byte name[256];
			GetVolumeInformationA(buf, name, 256, 0, 0, 0, 0, 0);

			printf("%s ", name);

			Drive_Type type = GetDriveTypeA(buf);
			switch(type)
			{
				case DRIVE_UNKNOWN:
					printf("unknown");
					break;

				case DRIVE_NO_ROOT_DIR:
					printf("no root dir");
					break;

				case DRIVE_REMOVABLE:
					printf("removable");
					break;

				case DRIVE_FIXED:
					printf("fixed");
					break;

				case DRIVE_REMOTE:
					printf("remote");
					break;

				case DRIVE_CDROM:
					printf("CD-ROM");
					break;

				case DRIVE_RAMDISK:
					printf("RAM disc");
					break;
			}

			printf(" %s", buf);
			printf("\n");
		}
	end*/
}


View view_1 = {
	.x = 0,
	.y = 0,
	.width = 300,
	.height = 300,
	.name = L"View 1"
};

Text text_1_1 = {
	.x = 0,
	.y = 64,
	.width = 100,
	.height = 100,
	.text = L"dkfjdaj ijiojadiojgioadiogjaio jgiojiojg iojs ioj giosdj"
};

Button button_1_1 = {
	.x = 0,
	.y = 0,
	.width = 100,
	.height = 32,
	.text = L"But ton"
};

Edit_Text edit_text_1_1 = {
	.x = 0,
	.y = 184,
	.width = 100,
	.height = 32,
	.text = L"Type text"
};

void view_1_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	switch(message)
	{
		case WM_COMMAND:
			if(parameter2 == button_1_1.window)
			{
				printf("button 1 click\n");
			}
			break;
	}
}


View view_2 = {
	.x = 0,
	.y = 320,
	.width = 300,
	.height = 300,
	.name = L"View 2"
};

Button button_2_1 = {
	.x = 10,
	.y = 10,
	.width = 100,
	.height = 24,
	.text = L"Exit"
};

Edit_Text edit_text_2_1 = {
	.x = 120,
	.y = 10,
	.width = 100,
	.height = 24,
	.text = L""
};

void view_2_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	switch(message)
	{
		case WM_COMMAND:
			if(parameter2 == button_2_1.window)
				PostQuitMessage(0);
			break;
	}
}


void main()
{
	//print_logical_drives();

	initialize_view(&view_1, &view_1_message_handler);

	create_text(&view_1, &text_1_1);
	create_button(&view_1, &button_1_1);
	create_edit_text(&view_1, &edit_text_1_1);

	initialize_view(&view_2, &view_2_message_handler);
	create_button(&view_2, &button_2_1);
	create_edit_text(&view_2, &edit_text_2_1);

	draw_views();
}