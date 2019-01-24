#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <windows.h>
using namespace std;

const char Vers[32] = "Beta 2.1.2.024";
const char Author[32] = "iBug";

/***********************
**                    **
**  Data & Functions  **
**                    **
***********************/

typedef unsigned char byte;
HANDLE hOut,hIn;
LCID langID;
void initConsole();
byte bitByUp(byte,int),bitByDown(byte,int);
bool bitBy(byte,int);
double getTime(void); 

// Control (Actually Input)
namespace Control{
	int GetMouse(COORD&,DWORD&);
	int GetMouseWait(COORD&,DWORD&);
}
namespace Display{
	const unsigned short gdStatusN = 2;
	byte gData[30][24],gStatus[gdStatusN];// For Minesweeper Board and Menu Bar Buttons
	bool SyncEnabled = true;// If set to false, disable "Sync" method [Experimental]
	unsigned short gMineLeft,gTime,gTimeMilli,gWidth,gHeight;// For Minesweeper Data
	const char *text;
	void SetMode(int);
	int Proceed(void);// Introduce the newest "Sync" method
	int Append(const char*,const COORD&);
	COORD toBoardPos(COORD),toScreenPos(COORD);
}

/****************
**             **
**  Main Game  **
**             **
****************/

namespace Game{
	const unsigned long ConfigNum = 3;
	byte board[30][24],mycfg[ConfigNum]={1,0,0};bool gameStarted,win;
	unsigned short mineLeft,mineCount,boardWidth,boardHeight,diff,Time,lang;
	unsigned long mouseBtn,mouseBtnPrev,mouseEvent,boardSize,TimeStart;
	COORD mousePos,mousePosPrev,Pos; signed long k;
	
	/*
	Memory Data Format:
	Bin: 0000 0000
	Higher 4 bits (0xF0):  128 for 'isLocked' (In certain cases, useful. And overwrite 'isUncovered' as well)
	                        64 for 'isDiscovered' (Overwrite 'isMarked')
	                        32 for 'isMarked' (Overwrite 'isMine')
	                        16 for 'isMine'
	Lower 4 bits (0x0F): Number of mines around (Ranges from 0 to 8, values 9 to 15 will lead to malfunctions)
	*/
	
	extern const char *MainMenu, (*ConfigText)[60];
	void SetLanguage(DWORD);
	bool ShowMainMenu = true;
	const byte boardConf[3][3] = {{9,9,10},{16,16,40},{30,16,99}};
	inline byte& Board(const COORD& Pos){return board[Pos.X][Pos.Y];}
	inline bool isValidPos(COORD Pos){return !(Pos.X<0 || Pos.X>=boardWidth || Pos.Y<0 || Pos.Y>=boardHeight);}
	using std::clock;using std::memcpy;int trigger(COORD,int);
	
	// For better code readability
	inline bool isLocked(const COORD& Pos){return bitBy(Board(Pos),7);}
	inline bool isUncovered(const COORD& Pos){return bitBy(Board(Pos),6);}
	inline bool isMarked(const COORD& Pos){return bitBy(Board(Pos),5);}
	inline bool isMine(const COORD& Pos){return bitBy(Board(Pos),4);}
	
	void ConfigMenuScreen(){
		Display::SetMode(0);
		string ConfigMenuText_str = "\x01\x1F[Back]\x01\x0F\n";
		for(int i=0;i<ConfigNum;i++)ConfigMenuText_str.append("\n[  ] ").append(ConfigText[i]);
		Display::text = ConfigMenuText_str.c_str();
		Display::Proceed();
		for(int i=0;i<ConfigNum;i++){
			COORD Pos = {0,(short)(2+i)};
			if(mycfg[i]){Display::Append("\x01\x2F[  ]",Pos);}
			else{Display::Append("\x01\x4F[  ]",Pos);}
		}
		mouseBtn = 0;mouseEvent = 0;signed short option;
		// Layer 1: Repeat until mouse click on [Back]
		for(;;){
			// Layer 2: Repeat until a mouse click
			for(;;){
				mouseBtnPrev = mouseBtn;
				Control::GetMouseWait(mousePos,mouseBtn);
				if(mouseBtnPrev!=0 && mouseBtn==0){
					if(mousePos.Y>=2 && mousePos.Y<2+ConfigNum && mousePos.X<4)break;// Option clicked
					if(mousePos.Y==0 && mousePos.X<6)break;// [Back] clicked
				}
			}
			option = mousePos.Y-2;
			if(option==-2){
				mouseBtn = mouseBtnPrev = 0;
				return;
			}
			mycfg[option] = !mycfg[option];
			if(mycfg[ConfigNum - 1]){SetLanguage(2052);}
			else {SetLanguage(1033);}
			mousePos.X = 0;
			if(mycfg[option]){Display::Append("\x01\x2F[  ]",mousePos);}
			else{Display::Append("\x01\x4F[  ]",mousePos);}
		}
	}
	void MainMenuScreen(){
		diff = -1;
		while(diff>3){
			// Clear screen and show main menu
			Display::SetMode(0);
			Display::text = MainMenu;
			Display::Proceed();
			// Wait until a button is clicked
			mouseBtn = 0;mouseEvent = 0;
			for(;;){
				mouseBtnPrev = mouseBtn;
				Control::GetMouseWait(mousePos,mouseBtn);
				if(mouseBtnPrev!=0 && mouseBtn==0){
					if(mousePos.Y>=5 && mousePos.Y<9 && mousePos.X<5)break;// Select difficulty
					if(mousePos.Y==2 && mousePos.X<10)break;// Settings screen
				}
			}
			mouseBtn = mouseBtnPrev = 0;
			diff = mousePos.Y-5;
			if(mousePos.Y==2)ConfigMenuScreen();
		}
		// On Settings Screen
		// On Custom Difficulty
		if(diff==3){
			Display::SetMode(0);
			Pos.X = 0;Pos.Y = 0;
			if(lang==1){
				Display::Append("请输入宽度: ",Pos);
			}else{
				Display::Append("Please Enter Columns: ",Pos);
			}
			do{
				cin.clear();
				cin>>boardWidth;
			}while(!cin);
			Display::SetMode(0);
			if(lang==1){
				Display::Append("请输入高度: ",Pos);
			}else{
				Display::Append("Please Enter Rows: ",Pos);
			}
			do{
				cin.clear();
				cin>>boardHeight;
			}while(!cin);
			Display::SetMode(0);
			if(lang==1){
				Display::Append("请输入雷数: ",Pos);
			}else{
				Display::Append("Please Enter Number of Mines: ",Pos);
			}
			do{
				cin.clear();
				cin>>mineCount;
			}while(!cin);
			// Process Data
			if(boardWidth<9)boardWidth = 9;
			if(boardWidth>30)boardWidth = 30;
			if(boardHeight<9)boardHeight = 9;
			if(boardHeight>24)boardHeight = 24;
			if(mineCount<10)mineCount = 10;
			if(mineCount>(boardWidth-1)*(boardHeight-1))mineCount = (boardWidth-1)*(boardHeight-1);
		}
	}
	int initGame(){
		// Configure Minesweeper Board
		if(diff<3){
			boardWidth = boardConf[diff][0];
			boardHeight = boardConf[diff][1];
			mineCount = boardConf[diff][2];
		}
		boardSize = boardWidth*boardHeight;mineLeft = mineCount;
		std::memset(board,0,720);
		std::memset(Display::gData,0,720);
		mouseBtn = 0;
		Display::gTime = 0;
		Display::gMineLeft = mineLeft;
		Display::gWidth = boardWidth;
		Display::gHeight = boardHeight;
		Display::SetMode(1);
		Display::SyncEnabled = false;
		Display::Proceed();
		Display::SyncEnabled = true;
		// Wait until a valid click
		mouseBtn = 0;mouseEvent = 0;
		while(mouseBtn!=0 || mouseBtnPrev!=1 || !(isValidPos(Pos) ||(mousePos.X<30 && mousePos.Y==0))){
			mouseBtnPrev = mouseBtn;
			Control::GetMouseWait(mousePos,mouseBtn);
			if(mousePos.Y>0){
				Pos.X = (mousePos.X-(60-2*boardWidth)/2)/2;
				Pos.Y = mousePos.Y-(24-boardHeight)/2-1;
				Display::gStatus[0] = 0;
				Display::gStatus[1] = 0;
			}else{
				if(mousePos.X<30){
					Pos.X = Pos.Y = -32768;
					Display::gStatus[1] = 1;
					Display::gStatus[0] = 0;
				}else{
					Pos.X = Pos.Y = -32767;
					Display::gStatus[0] = 1;
					Display::gStatus[1] = 0;
				}
			}
			Display::Proceed();
		}
		if(mousePos.X<30 && mousePos.Y==0){
			// Return button clicked
			ShowMainMenu = true;
			return 1;
		}
		mousePos = Pos;
		// Deploy mines
		// Step 1: Protect all 9 tiles from being mined [If Option 0 is enabled]
		if(mycfg[0]){
			for(signed short i=0;i<9;i++){
				Pos.X = mousePos.X+i%3-1;Pos.Y = mousePos.Y+i/3-1;
				if(isValidPos(Pos))Board(Pos) = bitByUp(Board(Pos),4);
			}
		}else{
			Board(mousePos) = bitByUp(Board(mousePos),4);
		}
		// Step 2: Deploy mines - Line[this+4] will avoid protected tiles
		for(signed short i=0;i<mineCount;i++){
			k = std::rand()%boardSize;
			Pos.X = k%boardWidth;Pos.Y = k/boardWidth;
			if(bitBy(Board(Pos),4)){i--;continue;}
			Board(Pos) = bitByUp(Board(Pos),4);
		}
		// Step 3: Remove protection [Condition same as Step 1]
		if(mycfg[0]){
			for(signed short i=0;i<9;i++){
				Pos.X = mousePos.X+i%3-1;Pos.Y = mousePos.Y+i/3-1;
				if(isValidPos(Pos))Board(Pos) = bitByDown(Board(Pos),4);
			}
		}else{
			Board(mousePos) = bitByDown(Board(mousePos),4);
		}
		// Calculate surrounding mines
		for(signed short i=0,j=0;i<boardSize;i++){
			k = 0;
			for(j=0;j<9;j++){
				if(j==4)continue;
				Pos.X = i%boardWidth+j%3-1;
				Pos.Y = i/boardWidth+j/3-1;
				if(!isValidPos(Pos))continue;
				if(bitBy(Board(Pos),4))k++;
			}
			board[i%boardWidth][i/boardWidth] |= (k&0x0F);
		}
		// Completing
		gameStarted = true;win = false;
		trigger(mousePos,0);
		Display::Proceed();
		TimeStart = getTime(); 
		return 0;
	}
	int trigger(COORD pos,int Mode){
		/* Parameter "Mode":
		0 - Left Click -> Open
		1 - Right Click -> Flag
		2 - Middle Click -> Open around if correctly flagged
		*/
		if(!isValidPos(pos))return -1; 
		if(bitBy(Board(pos),7))return -1;// Bit 'isLocked' is set
		COORD Pos;
		if(Mode == 0){
			if(Board(pos)&0xE0)return 0;
			if(bitBy(Board(pos),4)){
				Board(pos) = bitByUp(Board(pos),7);
				win = false;
				gameStarted = false;
				return 0;
			}
			Board(pos) = bitByUp(Board(pos),6);
			Display::gData[pos.X][pos.Y] = Board(pos)&0x0F;
			// On discovery of a "Zero"
			if((Board(pos)&0x0F)==0){
				Board(pos)=bitByUp(Board(pos),7);
				COORD Pos;
				Display::gData[pos.X][pos.Y] = 9;
				for(int k=0;k<9;k++){
					Pos.X = pos.X-1+k%3; Pos.Y = pos.Y-1+k/3;
					if(isValidPos(Pos))if(Board(Pos)<0x20)trigger(Pos,0);
				}
			}
		}
		if(Mode == 1){
			if(Board(pos)&0xC0)return 0;
			if(bitBy(Board(pos),5)){
				Board(pos) = bitByDown(Board(pos),5);
				mineLeft++;
				Display::gData[pos.X][pos.Y] = 0;
			}
			else{
				Board(pos) = bitByUp(Board(pos),5);
				mineLeft--;
				Display::gData[pos.X][pos.Y] = 11;
			}
			Display::gMineLeft = mineLeft;
		}
		if(Mode == 2){
			int i,k = 0;
			for(i=0;i<9;i++){
				if(i==4)continue;
				Pos.X = pos.X+i%3-1;Pos.Y = pos.Y+i/3-1;
				if(isValidPos(Pos))if(bitBy(Board(Pos),5))k++;
			}
			if(k!=(Board(pos)&0x0F))return 0;
			Board(pos) = bitByUp(Board(pos),7); // Now lock it
			for(i=0;i<9;i++){
				if(i==4)continue;
				Pos.X = pos.X+i%3-1;Pos.Y = pos.Y+i/3-1;
				if(isValidPos(Pos))
					if(bitBy(Board(Pos),4) ^ bitBy(Board(Pos),5)){
						win = false;
						gameStarted = false;
						return 0;
				}
			}
			for(i=0;i<9;i++){
				if(i==4)continue;
				Pos.X = pos.X+i%3-1;Pos.Y = pos.Y+i/3-1;
				if(isValidPos(Pos)){
					trigger(Pos,0);
					// Board(Pos) = bitByUp(Board(Pos),7);
				}
			}
		}
		return 0;
	}
	void Game(){
		SetLanguage(langID); 
		if(ShowMainMenu) MainMenuScreen();
		if(initGame()==1)return;
		COORD Pos; bool winchk;
		// Still in beta
		if( false ){
			// The Auto Minesweeper Game
			while(gameStarted){
				Sleep(100); // Limit animation speed
				
				Display::gTime = (getTime()-TimeStart)/1000;
				signed int cc;
				COORD pos;
				
				// Maybe this should be packed into a single AutoMove function
				int i;
				for(i=0;i<boardSize;i++){
					Pos.X = i%boardWidth;
					Pos.Y = i/boardWidth;
					if(isLocked(Pos) || !isUncovered(Pos) || isMarked(Pos))continue;
					cc = Board(Pos) & 0x0F;
					for(int j=0;j<9;j++){
						pos.X = Pos.X+j%3-1;
						pos.Y = Pos.Y+j/3-1;
						if(!isValidPos(pos))continue;
						if(!isUncovered(pos))cc--; // Count un-discovered tiles
					}
					if(cc==0){
						Board(Pos)=bitByUp(Board(Pos),7); // Bit 'isLocked' here is used as 'isChecked'
						// Mark them all ...
						for(int j=0;j<9;j++){
							pos.X = Pos.X+j%3-1;
							pos.Y = Pos.Y+j/3-1;
							if(!isMarked(pos))trigger(pos,1);
						}
						// ... and trigger
						for(int j=0;j<9;j++){
							pos.X = Pos.X+j%3-1;
							pos.Y = Pos.Y+j/3-1;
							trigger(pos,2);
						}
						break;
					}
				}
				// In case the first round fails
				if(i==boardSize){
					for(i=0;i<boardSize;i++){
						Pos.X = i%boardWidth;
						Pos.Y = i/boardWidth;
						if(isUncovered(Pos)){
							//char s[64];snprintf(s, sizeof(s), "Trigger %d,%d", Pos.X, Pos.Y, cc);
							//MessageBox(0,s,"Debug",MB_OK);
							trigger(Pos,2);
						}
					}
				}
				// Win Check
				winchk = true;
				for(i=0;i<boardSize;i++){
					Pos.X = i%boardWidth;Pos.Y = i/boardWidth;
					if(!bitBy(Board(Pos),4) && !bitBy(Board(Pos),6)){winchk = false;break;}
				}
				if(winchk){win = true;gameStarted = false;}
				// Completing
				Display::Proceed();
			}
		}
		else
		// A single loop in this 'while' is a single event
		while(gameStarted){
			mouseBtnPrev = mouseBtn;
			mousePosPrev = mousePos;
			// Clean up those indicator colors from last frame
			if(mouseEvent>2){ 
				for(int i=0;i<9;i++){
					Pos.X = (mousePos.X-(60-2*boardWidth)/2)/2+i%3-1;
					Pos.Y = mousePos.Y-(24-boardHeight)/2+i/3-2;
					if(isValidPos(Pos))
						Display::gData[Pos.X][Pos.Y] = bitBy(Board(Pos),6)?(Board(Pos)&0x0F?Board(Pos)&0x0F:9):(bitBy(Board(Pos),5)?11:0);
				}
			}else{
				Pos.X = (mousePos.X-(60-2*boardWidth)/2)/2;
				Pos.Y = mousePos.Y-(24-boardHeight)/2-1;
				if(isValidPos(Pos))
					Display::gData[Pos.X][Pos.Y] = bitBy(Board(Pos),6)?(Board(Pos)&0x0F?Board(Pos)&0x0F:9):bitBy(Board(Pos),5)?11:0;
			}
			
			Control::GetMouse(mousePos,mouseBtn);
			Display::gTime = (getTime()-TimeStart)/1000;
			// Process mouse position
			if(mousePos.Y>0){
				Pos.X = (mousePos.X-(60-2*boardWidth)/2)/2;
				Pos.Y = mousePos.Y-(24-boardHeight)/2-1;
				Display::gStatus[0] = 0;
				Display::gStatus[1] = 0;
			}else{
				// Mouse moved to function buttons
				if(mousePos.X<30){
					Pos.X = Pos.Y = -32768;
					Display::gStatus[1] = 1;
					Display::gStatus[0] = 0;
				}else{
					Pos.X = Pos.Y = -32767;
					Display::gStatus[0] = 1;
					Display::gStatus[1] = 0;
				}
			}
			// Trigger events
			if(mouseBtn==0){
				if(mouseEvent>3 && isValidPos(Pos))trigger(Pos,2);
				else if(mouseEvent>0 && isValidPos(Pos)){trigger(Pos,mouseEvent-1);}
				else if(mouseEvent==1 && mousePos.Y==0){
					if(mousePos.X<30){
						ShowMainMenu = true;
					}else{
						ShowMainMenu = false;
					}
					mouseBtnPrev = mouseEvent = 0;
					return;
				}
				mouseEvent = 0;
			}
			else{
				mouseEvent |= mouseBtn;
				if(isValidPos(Pos)){
					if(mouseEvent>2){
						COORD pos;
						for(int i=0;i<9;i++){
							pos.X = Pos.X+i%3-1;
							pos.Y = Pos.Y+i/3-1;
							if(isValidPos(pos) && Display::gData[pos.X][pos.Y]==0)Display::gData[pos.X][pos.Y] = 9;
						}
					}else if(mouseEvent!=2){
						if(Display::gData[Pos.X][Pos.Y]==0)Display::gData[Pos.X][Pos.Y] = 9;
					}
				}
			}
			// Win Check
			winchk = true;
			for(int i=0;i<boardSize;i++){
				Pos.X = i%boardWidth;Pos.Y = i/boardWidth;
				if(!bitBy(Board(Pos),4) && !bitBy(Board(Pos),6)){winchk = false;break;}
			}
			if(winchk){win = true;gameStarted = false;}
			// Completing
			Display::Proceed();
		}
		if(win){
			for(int i=0;i<boardSize;i++){
				Pos.X = i%boardWidth;Pos.Y = i/boardWidth;
				// Make all mines visible as "Marked"
				if(bitBy(Board(Pos),4)){Display::gData[Pos.X][Pos.Y] = 11;}
			}
		}else{
			for(int i=0;i<boardSize;i++){
				Pos.X = i%boardWidth;Pos.Y = i/boardWidth;
				// Display all unfound mines and incorrect marks
				if(bitBy(Board(Pos),4) && !bitBy(Board(Pos),5))Display::gData[Pos.X][Pos.Y] = bitBy(Board(Pos),7)?14:10;
				if(bitBy(Board(Pos),5) && !bitBy(Board(Pos),4))Display::gData[Pos.X][Pos.Y] = bitBy(Board(Pos),7)?14:12;
			}
		}
		// Show millisecond (Which has been zero since game start)
		int gameTime = getTime()-TimeStart;
		Display::gTimeMilli = gameTime>1000000?999:gameTime%1000;
		Display::Proceed();
		// Keep the board until either button is clicked
		for(;;){
			if(Pos.X==-32768 && Pos.Y==-32768 && mouseBtn==0 && mouseBtnPrev!=0){ShowMainMenu = false;break;}
			if(Pos.X==-32767 && Pos.Y==-32767 && mouseBtn==0 && mouseBtnPrev!=0){ShowMainMenu = true;break;}
			
			mouseBtnPrev = mouseBtn;
			Control::GetMouseWait(mousePos,mouseBtn);
			if(mousePos.Y>0){
				Pos.X = (mousePos.X-(60-2*boardWidth)/2)/2;
				Pos.Y = mousePos.Y-(24-boardHeight)/2-1;
				Display::gStatus[0] = 0;
				Display::gStatus[1] = 0;
			}else{
				if(mousePos.X<30){
					Pos.X = Pos.Y = -32767;
					Display::gStatus[1] = 1;
					Display::gStatus[0] = 0;
				}else{
					Pos.X = Pos.Y = -32768;
					Display::gStatus[0] = 1;
					Display::gStatus[1] = 0;
				}
			}
			Display::Proceed();
		}
	}
	
}

int main(){
	initConsole();
	for(;;) Game::Game();
}

/**************
**           **
**  Library  **
**           **
**************/

void initConsole(){
	// General Initializer
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	// DeleteMenu(GetSystemMenu(GetConsoleWindow(),0),SC_MAXIMIZE,MF_DISABLED);
	// Get Language ID
	langID = GetSystemDefaultLCID();
	if(langID == 2052) Game::mycfg[Game::ConfigNum - 1] = 1;
	// Resize Screen
	// Notice: Default Window 80x25, Default Buffer 80x300
	COORD bSize = {60,27};
	SMALL_RECT cSize = {0,0,59,26};
	SetConsoleWindowInfo(hOut,1,&cSize);
	SetConsoleScreenBufferSize(hOut,bSize);
	SetConsoleWindowInfo(hOut,1,&cSize);
	// Set Title
	char title[64];
	if(langID==2052){
		snprintf(title,sizeof(title),"扫雷II %s",Author);
	}else{
		snprintf(title,sizeof(title),"Minesweeper 2 %s",Author);
	}
	SetConsoleTitle(title);
	// Hide Cursor [_] -> [ ]
	CONSOLE_CURSOR_INFO CCI;GetConsoleCursorInfo(hOut,&CCI);
	CCI.bVisible = 0;SetConsoleCursorInfo(hOut,&CCI);
	// Initialize Randomizer
	srand((unsigned)time(NULL));
}
inline double getTime(void){
	static LARGE_INTEGER freq,counter; 
    QueryPerformanceFrequency(&freq);   
    QueryPerformanceCounter(&counter);  
    return 1.0e3*counter.QuadPart/freq.QuadPart;
}

namespace Display{
	int Mode = 0;unsigned long clsCount = 0;
	byte dData[30][24],dStatus[gdStatusN];
	HANDLE thread;
	unsigned short dMineLeft,dTime,dTimeMilli,dWidth,dHeight;
	COORD gMineLeftPos,gTimePos,gTimeMilliPos,gOffset,Pos;
	const char img[256][3] = {"  ","①","②","③","④","⑤","⑥","⑦","⑧","  ","¤","▲","×","  ","¤","  "};
	//               Meaning: None   1    2    3    4    5    6    7    8  Safe Mine Mark Wrong SP  Mine 
	const WORD imgColor[256]={0x1F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x4F,0x1F,0xEC,0x00,0xEC,0x00};
	//                Number:   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
	const char btnimg[16][3] = {"★","←"};
	const WORD btnimgColor[16]={0x8E,0x89};
	const WORD btnimgClrBr[16]={0x7E,0x79};
	//                  Number:   0    1
	const COORD btnPos[16] = {{30,0},{28,0}};
	
	void clear(){
		COORD startPos = {0,0};
		FillConsoleOutputCharacter(hOut,0x20,1560,startPos,&clsCount);
		FillConsoleOutputAttribute(hOut,0x0F,1560,startPos,&clsCount);
		SetConsoleCursorPosition(hOut,startPos);
	}
	COORD toBoardPos(COORD Pos){
		COORD POS;
		if(Pos.X<gOffset.X || Pos.Y<gOffset.Y){
			if(Pos.X<30 && Pos.Y==0)POS.X = POS.Y = -32767;
			else if(Pos.X>=30 && Pos.Y==0)POS.X = POS.Y = -32768;
			else POS.X = POS.Y = -1;
		}else{
			POS.X = (Pos.X-gOffset.X)/2;
			POS.Y = Pos.Y-gOffset.Y;
			if(!Game::isValidPos(POS))POS.X = POS.Y = -1;
		}
		return POS;
	}
	COORD toScreenPos(COORD Pos){
		COORD POS;
		if(Pos.X==-32768 && Pos.Y==-32768){POS.X = 28;POS.Y = 0;}
		else if(Pos.X==-32767 && Pos.Y==-32767){POS.X = 30;POS.Y = 0;}
		else if(!Game::isValidPos(Pos))POS.X = POS.Y = -1;
		else{POS.X = Pos.X*2+gOffset.X;POS.Y = Pos.Y+gOffset.Y;}
		return POS; 
	}
	void SetMode(int newMode){
		Mode = newMode;
		if(newMode==1){
			dWidth = gWidth;dHeight = gHeight;
			dMineLeft = 0;
			dTime = dTimeMilli = 999;
			gTime = gTimeMilli = 0;
			for(int i=0;i<gdStatusN;i++){
				dStatus[i] = 255;
				gStatus[i] = 0;
			}
			gMineLeftPos.X = 0;gMineLeftPos.Y = 0;
			gTimePos.X = 53;gTimePos.Y = 0;
			gTimeMilliPos.X = 56;gTimeMilliPos.Y = 0;
			gOffset.X = (60-2*gWidth)/2;
			gOffset.Y = (24-gHeight)/2;
			std::memset(dData,255,720);
			Pos.X = 29;Pos.Y = 0;
		}
		clear();
	}
	int Proceed(void){
		static int i,j;
		using namespace std;
		switch(Mode){
			case 0:
				clear();
				SetConsoleTextAttribute(hOut,0x0F);
				for(i=0;text[i];i++){
					if(text[i]==0x01){i++;SetConsoleTextAttribute(hOut,text[i]);i++;}
					cout<<text[i];
				}
				break;
			case 1:
				for(i=0;i<gdStatusN;i++){
					if(dStatus[i] != gStatus[i]){
						SetConsoleCursorPosition(hOut,btnPos[i]);
						SetConsoleTextAttribute(hOut,(dStatus[i]=gStatus[i])==0?btnimgColor[i]:btnimgClrBr[i]);
						cout<<btnimg[i];
					}
				}
				cout<<setfill('0');
				// Print MineLeft and Time 
				SetConsoleTextAttribute(hOut,0x0F);
				if(gMineLeft != dMineLeft){
					SetConsoleCursorPosition(hOut,gMineLeftPos);
					cout<<setw(3)<<(gMineLeft=dMineLeft=gMineLeft>999?999:gMineLeft);
				}
				if((gTime=gTime>999?999:gTime) != dTime){
					SetConsoleCursorPosition(hOut,gTimePos);
					cout<<setw(3)<<(dTime=gTime);
				}
				if(dTimeMilli != gTimeMilli){
					SetConsoleCursorPosition(hOut,gTimeMilliPos);
					cout<<'.'<<setw(3)<<(dTimeMilli=gTimeMilli);
				}
				// Print the board
				cout<<setfill(' ');
				if(SyncEnabled){
					for(j=0;j<dHeight;j++){
						for(i=0;i<dWidth;i++){
							if(dData[i][j]!=gData[i][j]){
								Pos.X = gOffset.X+i*2;Pos.Y = gOffset.Y+j+1;
								SetConsoleCursorPosition(hOut,Pos);
								SetConsoleTextAttribute(hOut,imgColor[gData[i][j]]);
								cout<<setw(2)<<img[gData[i][j]];
								dData[i][j] = gData[i][j];
							}
						}
					}
				}else{
					for(j=0;j<dHeight;j++){
						Pos.X = gOffset.X;Pos.Y = gOffset.Y+j+1;
						SetConsoleCursorPosition(hOut,Pos);
						for(i=0;i<dWidth;i++){
							SetConsoleTextAttribute(hOut,imgColor[gData[i][j]]);
							cout<<setw(2)<<img[gData[i][j]];
							dData[i][j] = gData[i][j];
						}
					}
				}
				break;
		}
		return 0;
	}
	int Append(const char* text,const COORD& Pos){
		SetConsoleCursorPosition(hOut,Pos);
		SetConsoleTextAttribute(hOut,0x0F);
		for(int i=0;text[i];i++){
			if(text[i]==0x01){i++;SetConsoleTextAttribute(hOut,text[i]);i++;}
			cout<<text[i];
		}
		return 0;
	}
	/*
	int AppendEx(const COORD& Pos,const char* text, const char* ... texts ...){
		Append(text,Pos);
		AppendEx(Pos, texts...);
	}
	*/
}
namespace Control{
	unsigned long readCount = 0,inputCount;
	INPUT_RECORD inRec;
	COORD GetConsoleCursorPosition(HANDLE H){CONSOLE_SCREEN_BUFFER_INFO A;GetConsoleScreenBufferInfo(H,&A);return A.dwCursorPosition;}
	int GetMouse(COORD &Pos,DWORD &btnState){
		GetNumberOfConsoleInputEvents(hIn,(LPDWORD)&inputCount);
		if(inputCount==0)return 1;// Nothing Happened
		ReadConsoleInput(hIn,&inRec,1,&readCount);
		if(inRec.EventType != MOUSE_EVENT)return 2;// Not a Mouse Event
		Pos = inRec.Event.MouseEvent.dwMousePosition;
		btnState = inRec.Event.MouseEvent.dwButtonState;
		return 0;// Well Done
	}
	int GetMouseWait(COORD &Pos,DWORD &btnState){
		ReadConsoleInput(hIn,&inRec,1,&readCount);
		if(inRec.EventType != MOUSE_EVENT)return 2;// Not a Mouse Event
		Pos = inRec.Event.MouseEvent.dwMousePosition;
		btnState = inRec.Event.MouseEvent.dwButtonState;
		return 0;// Well Done
	}
}

namespace Game{
	string MainMenuText_str_CN = 
		string("欢迎来到雷神 ")
		.append(::Vers)
		.append("\n作者：")
		.append(::Author)
		.append("\n\x01\x0E[选项设置]\x01\x0F")
		.append("\n\n选择难度:\n")
		.append("\x01\x2F[ 1 ]\x01\x0F 简单\n")
		.append("\x01\x1F[ 2 ]\x01\x0F 中等\n")
		.append("\x01\x4F[ 3 ]\x01\x0F 困难\n")
		.append("\x01\x70[ 4 ]\x01\x0F 自定义\n")
		.append("鼠标点选");
	string MainMenuText_str_EN = 
		string("Welcome to Minesweeper II ")
		.append(::Vers)
		.append("\nAuthor: ")
		.append(::Author)
		.append("\n\x01\x0E[Settings]\x01\x0F")
		.append("\n\nSelect Difficulty:\n")
		.append("\x01\x2F[ 1 ]\x01\x0F Easy\n")
		.append("\x01\x1F[ 2 ]\x01\x0F Medium\n")
		.append("\x01\x4F[ 3 ]\x01\x0F Hard\n")
		.append("\x01\x70[ 4 ]\x01\x0F Custom\n")
		.append("Click on your choice.");
		
	const char ConfigText_EN[ConfigNum][60] = { "First Click On Empty",
												"Enable Automation AI", 
												"切换到中文界面"};
	const char ConfigText_CN[ConfigNum][60] = { "第一次点击为空白",
												"使用自动扫雷AI算法", 
												"Use Simplified Chinese Interface"};
	
	const char (*ConfigText)[60], *MainMenu;
	void SetLanguage(DWORD lang){
		if(lang==2052){
			Game::lang = 1; 
			MainMenu = MainMenuText_str_CN.c_str();
			ConfigText = ConfigText_CN;
		}else{
			Game::lang = 0;
			MainMenu = MainMenuText_str_EN.c_str();
			ConfigText = ConfigText_EN;
		}
	}
}

inline byte bitByUp(byte a,int n){return a|(1<<n);}
inline byte bitByDown(byte a,int n){return a&~(1<<n);}
inline bool bitBy(byte a,int n){return (a&(1<<n))!=0;}

