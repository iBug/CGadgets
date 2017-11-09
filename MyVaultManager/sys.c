// File: sys.c
// Author: iBug

#include "sys.h"

#include <stdio.h>

inline void clearInputBuffer(void){
	while((getchar())!='\n');
}

#ifdef _WIN32
#include <windows.h>
#include <conio.h>

inline void pause(void){
	clearKeyBuffer();
	while (!kbhit());
	clearKeyBuffer();
}

inline void clearKeyBuffer(void){
	while (kbhit()) getch();
}

void clearScreen(void){
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD startPos = {0,0};
	DWORD clsCount, size;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOutput, &csbi);
	size = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hOutput, 0x20, size, startPos, &clsCount);
	FillConsoleOutputAttribute(hOutput, 0x07, size, startPos, &clsCount);
	SetConsoleCursorPosition(hOutput,startPos);
}

#endif // _WIN32

#ifdef __unix__
#include <termios.h>
#define STDIN_FILENO 0 

void pause(void){
	struct termios savedState, newState;
	int c, echo = 0;
	tcgetattr(STDIN_FILENO, &savedState);
	newState = savedState;
    if ((echo = !echo)){
		echo = ECHO;
    }
    newState.c_lflag &= ~(echo | ICANON);
    newState.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &newState);
    c = getchar();
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &savedState);
    return;
}

void clearScreen(void){
	printf("\x1B[2J");
}

inline void clearKeyBuffer(void){
}
#endif // __unix__
