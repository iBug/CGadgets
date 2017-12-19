@path %ProgramFiles%\Dev-Cpp\MinGW32\bin;%Path%
mingw32-make.exe -f "D:\.File\Private\Project\C\Editor\Makefile.win" all

g++.exe -c main.cpp -o main.o -I"C:/Program Files/Dev-Cpp/MinGW32/include" -I"C:/Program Files/Dev-Cpp/MinGW32/mingw32/include" -I"C:/Program Files/Dev-Cpp/MinGW32/lib/gcc/mingw32/4.8.1/include/c++" -std=c++11
windres.exe -i Editor_private.rc --input-format=rc -o Editor_private.res -O coff 
g++.exe main.o Editor_private.res -o Editor.exe -L"C:/Program Files/Dev-Cpp/MinGW32/lib" -L"C:/Program Files/Dev-Cpp/MinGW32/mingw32/lib" -static-libstdc++ -static-libgcc -mwindows
pause