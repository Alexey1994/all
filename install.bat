@assoc .c=
@ftype runnable_c_application=
@reg delete HKCR\.c /f
@reg delete HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.c /f

@set TCC_PATH=%~dp0tcc

@ftype runnable_c_application="%TCC_PATH%\tcc.exe" -w ^
	"%TCC_PATH%\include\system\Windows\kernel32.def" ^
	"%TCC_PATH%\include\system\Windows\winmm.def" ^
	"%TCC_PATH%\include\system\Windows\gdi32.def" ^
	"%TCC_PATH%\include\system\Windows\user32.def" ^
	"%TCC_PATH%\include\system\Windows\opengl32.def" ^
	"%TCC_PATH%\include\system\Windows\avicap32.def" ^
	"%TCC_PATH%\include\system\Windows\ws2_32.def" ^
	"%TCC_PATH%\sqlite\sqlite3.def" ^
	-run %%1
@assoc .c=runnable_c_application
@setx path "%PATH%;%TCC_PATH%"