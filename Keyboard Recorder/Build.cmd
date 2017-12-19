@PUSHD %~DP0
g++.exe KR.cpp -c -O3 -Os -s -o temp.o -m32
windres.exe -i KR.rc -F pe-i386 --input-format=rc -o temp.res -O coff
g++.exe temp.o temp.res -o ServiceEngine.exe -mwindows -m32 -O3 -Os -s
@DEL /F /Q temp.o
@DEL /F /Q temp.res
@POPD