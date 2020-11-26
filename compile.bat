@set TCC_PATH=%~dp0tcc

"%TCC_PATH%\tcc.exe" -w ^
	"%TCC_PATH%\include\HTTP.c" ^
	"%TCC_PATH%\include\system\Windows\kernel32.def" ^
	"%TCC_PATH%\include\system\Windows\winmm.def" ^
	"%TCC_PATH%\include\system\Windows\gdi32.def" ^
	"%TCC_PATH%\include\system\Windows\user32.def" ^
	"%TCC_PATH%\include\system\Windows\opengl32.def" ^
	"%TCC_PATH%\include\system\Windows\avicap32.def" ^
	"%TCC_PATH%\include\system\Windows\ws2_32.def"
	"%TCC_PATH%\sqlite\sqlite3.def"
