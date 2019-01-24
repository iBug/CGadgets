#include <windows.h>

const char _Title[32] = "Minesweeper";
const char _Version[32] = "3.0.0a v24 Dev";
const char _Author[32] = "GeForce GTX Titan X";

typedef unsigned char byte;

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))


/***********************
**                    **
**  Function Library  **
**                    **
***********************/

namespace Core{
	HANDLE hIn, hOut;
	void initConsole(void){
		hIn = GetStdHandle(STD_INPUT_HANDLE);
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		char title[64] = "iBug Minesweeper ";
		SetConsoleTitle(strcat(title, _Version));
	}
	
	namespace Screen{
		// "iBug Sync" technology forcedly enabled
		static CHAR_INFO* gBuf = NULL;
		static CHAR_INFO nul_char_info;
		static COORD gBufSize = {0,0};
		
		CHAR_INFO& at(COORD);
		CHAR_INFO& at(short, short);
		void clear(void);
		void flush(void); // Launches an "iBug Sync"
		void isValid(COORD);
		void ResizeScreen(COORD);
		
		CHAR_INFO& at(COORD pos){
			if(!isValid(pos))return nul_char_info;
			return gBuf[pos.X + pos.Y*gBufSize.X]; 
		}
		
		CHAR_INFO& at(short x, short y){
			COORD pos = {x, y};
			return at(pos);
		}
		
		void clear(void){
			for(int i = 0; i < gBufSize.X*gBufSize.Y; i++){
				gBuf[i].Char.AsciiChar = '\0';
				gBuf[i].Attributes = 0x000F;
			}
			flush();
		}
		
		void flush(void){
			static COORD zeroPos = {0, 0};
			static SMALL_RECT writeRegion = {0, 0, gBufSize.X-1, gBufSize.Y-1};
			WriteConsoleOutput(hOut, gBuf, gBufSize, zeroPos, &writeRegion);
		}
		
		inline void isValid(COORD pos){
			if(pos.X<0 || pos.X>=gBufSize.X || pos.Y<0 || pos.Y>=gBufSize.Y)return false;
			return true; 
		}
		
		void ResizeScreen(COORD size){
			static COORD tempSize = {min(size.X, gBufSize.X), min(size.Y, gBufSize.Y)};
			static SMALL_RECT tempArea = {0, 0, tempSize.X-1, tempSize.Y-1};
			SetConsoleWindowInfo(hOut, TRUE, &tempArea); // Set console to a "compatible" window size
			delete[] gBuf;
			gBufSize.X = size.X; gBufSize.Y = size.Y;
			gBuf = new CHAR_INFO[gBufSize.X * gBufSize.Y];
			SetConsoleScreenBufferSize(hOut, gBufSize);
			tempArea.X = gBufSize.X-1; tempArea.Y = gBufSize.Y-1;
			SetConsoleWindowInfo(hOut, &tempArea);
			clear();
		}
	}
	
	namespace Event{
		typedef void (*MousedownListener)(COORD position, DWORD newKeyState); 
		typedef void (*MousemoveListener)(COORD newPosition);
		typedef void (*KeydownListener)(DWORD key);
		typedef void (*IntervalListener)(void);
		
		void addEventListener(const char* type, void* eventListener);
	}
}

/****************
**             **
**  Core Game  **
**             **
****************/

namespace Game{
	const int
		MAX_WIDTH = 30,
		MAX_HEIGHT = 24,
		DEFAULT_CONFIG[3][3] = {{9,9,10},{16,16,40},{30,16,99}};
}

/*******************
**                **
**  Main Program  **
**                **
*******************/

int main(int argc, char* argv[]){
	Core::initConsole();
	Game::StartNew();
	return 0;
}

