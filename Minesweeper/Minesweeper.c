#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define _Compatibility
#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0)
#define ScreenX 30
#define ScreenY 24
#define Color(a) SetConsoleTextAttribute(hOut,(a))
#define Board(A) board[(A).X][(A).Y]
#define Data(A) data[(A).X][(A).Y]
#define random (rand())
#define boardW 40
#define boardH 30
#define boardS (boardW*boardH)


// Library Part
const char Vers[32] = "Alpha 1.7.015";

typedef unsigned char byte;
//const char img[14][3] = {"  ","¢Ù","¢Ú","¢Û","¢Ü","¢Ý","¢Þ","¢ß","¢à","  ","©I","¡è","¡Á","¡ï"}; // Original Theme
const char img[14][3] = {"  ","£±","£²","£³","£´","£µ","£¶","£·","£¸","  ","¡Ñ","¡è","¡Á","¡ï"};
//             Meaning: Empty  1    2    3    4    5    6    7    8   Safe Mark Mine Wrong SP  >
//              Number:   0    1    2    3    4    5    6    7    8    9    10   11   12   13  >
//const WORD imgColor[14]={0x70,0x09,0x0A,0x0C,0x0B,0x0C,0x09,0x0F,0x0D,0x0F,0x70,0x4E,0x4E,0xE9}; // Light Theme (TBD)
const WORD imgColor[14]={0x1F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x2F,0x1F,0x4F,0xEC,0xE9}; // Original Theme
const char diffs[7][7] = {"Rookie","Easy  ","Medium","Hard  ","Expert","Master","Custom"};
const char sw_tx[2][4] = {"OFF","ON"};
HANDLE hIn,hOut;
HWND hWnd;
INPUT_RECORD inRec;
DWORD readCount, clsCount, inC;
HWND WINAPI WINBASEAPI GetConsoleWindow(void);
_CRTIMP void __cdecl _sleep(unsigned long _Duration) __MINGW_ATTRIB_DEPRECATED;

void initConsole(void),clrscr(void),clearKeyBuf(void),pause(),SetConsoleSize(COORD);
long getMouseEvent(COORD*,long),__time__,__prev__;
int in_Mouse(COORD*);
COORD GetConsoleCursorPosition(HANDLE);
const COORD CCP = {0,1};

// Data Definition Part 

byte board[boardW][boardH],data[boardW][boardH],GameStarted=1,GameEnded=0,diff;
byte mycfg[4] = {1,0,0,0};
const byte boardSX[6] = { 8, 9,16,30, 30, 30};
const byte boardSY[6] = { 8, 9,16,16, 20, 24};
const long minePwr[6] = { 6,10,40,99,130,166};
unsigned int boardX,boardY,mineCount,mineLeft,win=0,tick,sTick;
signed int ME,MEA,MES,MEPR;
COORD minePos,mousePos,mousePosPrev,losePos,timePos;
int isValidPos(COORD),CoordCmp(COORD,COORD);

/****************
**             **
**  Main Game  **
**             **
****************/

void printBoard(int),printSingle(COORD,int),Config(int);
int trigger(COORD,int);
inline int /*getMines(COORD),*/getRealMines(COORD),getMarked(COORD),getNonMine(COORD);
int GetPref(void){
	COORD cSize = {30,20}; 
	SetConsoleSize(cSize);
	clrscr();
	printf("Welcome to iBug Minesweeper!\nVersion %s\n",(const char*)Vers);
	puts("\nPress [SPACE] to Settings\n");
	puts("Select difficulty:");
	int i;
	for(i=0;i<6;i++){
		printf("\t%d. %-6s  %2dx%2d, %3d Mines\n", 1+i,diffs[i],boardSX[i],boardSY[i],minePwr[i]);
	}
	printf("\t7. Custom  <Unavailable>");
	clearKeyBuf();
	byte keyRead, keyProc = 1;
	while(keyProc==1){
		keyRead = getch();
		switch(keyRead){
			case 0x31:case 0x32:case 0x33:case 0x34:case 0x35:case 0x36:
				diff = keyRead-49;
				boardX = boardSX[diff];boardY = boardSY[diff];
				mineCount = minePwr[diff];
				keyProc = 0;break;
			case 0x20:
				Config(0);keyProc = 2;break;
			case 0x00:case 0xE0:
				getch();break;
		}
	}
	if(keyProc>1)return 1; 
	return 0;
}
void initGame(void){
	GameStarted=1;GameEnded=0;
	int bs;
	memset(board,0,sizeof(board));
	memset(data,0,sizeof(data));
	while(GetPref());
	COORD cSizeNew = {boardX>30?boardX:30,boardY},n;
	SetConsoleSize(cSizeNew);
	clrscr();
	Color(((mycfg[2]|mycfg[3])==0)?0x0F:0x0E);
	mineLeft = mineCount;
	printf("Time: ");
	timePos = GetConsoleCursorPosition(hOut);
	printf("%04d",0);
	printf(" %6s, Col: %2d, Row: %2d, Mines: ",diffs[diff],boardX,boardY);
	minePos = GetConsoleCursorPosition(hOut);
	Color(((mycfg[2]|mycfg[3])==0)?0x0F:0x0E);
	printf("%2d\n",mineLeft);
	printBoard(0); 
	signed int i,j,k,dx,dy,sx,sy,ml;
	while(in_Mouse(&mousePos)!=1)Sleep(10);
	for(j=0;j<boardY;++j)for(i=0;i<boardX;++i)board[i][j]=0;
	Board(mousePos) = 9;
	ml = 0;
	//Generating Mines
	if(diff>2 && mycfg[1]!=0){
		for(;;){
			sx=random%boardX;sy=random%boardY;
			dx = mousePos.X-sx;dy = mousePos.Y-sy;
			if((sx>2&&sx<boardX-3 && sy>2&&sy<boardY-3)&&!(dx>=-2 && dx<=2 && dy>=-2 && dy<=2)){
				for(int tk=0;tk<9;tk++){
					if(tk==4)continue;
					board[sx-1+tk%3][sy-1+tk/3] = 11;
				}
				board[sx][sy] = 8;
				ml += 8;
				break;
			}
		}
	}
	for(;ml<mineCount;){
		j=rand()%boardX;
		k=rand()%boardY;
		dx = mousePos.X-j;dy = mousePos.Y-k;
		if(mycfg[0] && (dx>=-1 && dx<=1 && dy>=-1 && dy<=1))continue;
		if(board[j][k]==0){board[j][k] = 11;++ml;}
	}
	if(diff>2 && mycfg[1]!=0)board[sx][sy] = 0;
	Board(mousePos) = 0;
	sTick = clock();
	trigger(mousePos,0);
	//Disabled printBoard(0);
}
int trigger(COORD Pos,int Mode){
	//Variable "Mode":
	//  0 LClick = Open
	//  1 RClick = Flag
	//  2 DoubleClick = AutoOpen
	//  3 Re-Trigger = When Mined Set Lose #Experimental
	//  4 AutoOpen = Used when mycfg[2]==1 #Experimental
	if(isValidPos(Pos)==0)return -1;
	Board(Pos) &= 0x7F;
	if(Mode==0){
		int mines = getRealMines(Pos);
		if(Board(Pos)==0)Board(Pos) = mines;
		else if(Board(Pos)==11){GameEnded=1;losePos=Pos;win=0;return 0;}
		else goto endTrigger;
		if(mines==9){
			int i,j;
			COORD newPos;
			for(i=0;i<9;i++){if(i==4)continue;
				newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
				if(Board(newPos)==0)trigger(newPos,0);
			}
			if(mycfg[2]|mycfg[3]==1){
				for(i=0;i<9;i++){if(i==4)continue;
					newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
					if(getRealMines(newPos)==9&&Board(newPos)==0)trigger(newPos,0);
				}
				for(i=0;i<9;i++){if(i==4)continue;
					newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
					if(Board(newPos)==0)trigger(newPos,2);
				}
			}
		}
		else if(mycfg[2]|mycfg[3]==1)trigger(Pos,2);
	}
	if(Mode==1){
		if(Board(Pos)==11)Board(Pos)=10;//Right Mark
		else{if(Board(Pos)==10)Board(Pos)=11;}//Remove Mark
		//Seperator-----
		if(Board(Pos)==0)Board(Pos)=12;//Wrong Mark 
		else{if(Board(Pos)==12)Board(Pos)=0;}//Remove Mark
		//Check Mines
		byte toCheck;
		unsigned int i,mxi=(unsigned int)(boardX*boardY),mnCnt=mineCount;
		for(i=0;i<mxi;++i){toCheck = board[i%boardX][i/boardX];
		switch(toCheck){case 10:case 12:--mnCnt;break;}}
		SetConsoleCursorPosition(hOut,minePos);
		Color(((mycfg[2]|mycfg[3])==0)?0x0F:0x0E);
		printf("%3d",mnCnt);
		COORD newPos;
		if(mycfg[2]|mycfg[3]==1){
		for(i=0;i<9;i++){
			if(i==4)continue;
			newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
			trigger(newPos,2);
		}}
	}
	if(Mode==2){
		if(!(Board(Pos)>0 && Board(Pos)<10))return 0;
		if(getMarked(Pos)==getRealMines(Pos)){
			// Firstly Check If All Mark Matches
			int i;COORD newPos;
			for(i=0;i<9;i++){
				if(i==4)continue;
				newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
				if(!isValidPos(newPos))continue;
				if(Board(newPos)>=11){losePos=newPos;GameEnded=1;win=0;return 0;}//Encountering A Mine
			}
			// Then Trigger Others
			for(i=0;i<9;i++){
				if(i==4)continue;
				newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
				trigger(newPos,3);
			}
			goto endTrigger;
		}
		// Then It's Switch[3]: The Legend Power of Silver
		else if(mycfg[3]==1){
			// If already "Activated"
			if(Data(Pos)&0x80!=0)goto endTrigger;
			// Only if a surrounding "Zero" already exists
			int flag = 0;
			int i;COORD newPos;
			for(i=0;i<9;i++){if(i==4)continue;
				newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
				if(Board(newPos)==9)flag = 1;
			}
			if(flag==0)goto endTrigger;
			// Clear All surrounding "Zero"s
			for(i=0;i<9;i++){if(i==4)continue;
				newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
				if(Board(newPos)==11)trigger(newPos,1);
			}
			// Only if all non-mine is open
			if(Board(Pos)+getNonMine(Pos)==8){
				for(i=0;i<9;i++){if(i==4)continue;
					newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
					trigger(newPos,2);
				}
			}
			Data(Pos) |= 0x80;
		}
	}
	if(Mode==3){
		return trigger(Pos,0);
		/*
		if(Board(Pos)==10)goto endTrigger;
		int mines = getRealMines(Pos);
		if(Board(Pos)>=11){GameEnded = 1;win=0;goto endTrigger;}
		if(mines==9){
			Board(Pos) = mines;
			int i,j;
			COORD newPos;
			for(i=-1;i<=1;i++)for(j=-1;j<=1;j++){
				if(i==0 && j==0)continue;
				newPos.X=Pos.X+i;newPos.Y=Pos.Y+j;
				if(Board(newPos)==0)trigger(newPos,3);
			}
		}else Board(Pos) = mines;
		*/
	}
	if(Mode==4){
		int r = trigger(Pos,2),i;
		COORD newPos;
		if(r==0)goto endTrigger;
		if(r==1)for(i=0;i<9;i++){
			if(i==4)continue;
			newPos.X=Pos.X-1+i%3;newPos.Y=Pos.Y-1+i/3;
			trigger(newPos,2);
		}
	}
	endTrigger:
	printSingle(Pos,0);
	return 0;
}
int hasWon(void){
	int Return = 1,i,j;
	for(j=0;j<boardY;++j)for(i=0;i<boardX;++i){
		switch(board[i][j]){
			case 0:
			case 12:
				Return = 0;
				break;
		}
	}
	return Return;
}
void Game(void){
	initGame();
	for(;!GameEnded;){
		//Disabled printBoard(0);
		while(in_Mouse(&mousePos)==0){
			tick = clock();__prev__=__time__;
			__time__ = (tick-sTick)/1000;
			if(__time__>9999)__time__ = 9999;
			SetConsoleCursorPosition(hOut,timePos);
			Color(mycfg[2]==0?0x0F:0x0E);
			if(__time__!=__prev__)printf("%04d",__time__);
		}
		trigger(mousePos,ME>3?2:ME-1);
		if(hasWon()){win=1;break;}
	}
	printBoard(1);
	{COORD cPos = minePos;cPos.X+=4;
	SetConsoleCursorPosition(hOut,cPos);}
	if(win==1){
		puts("You Win!");
	}
	else{
		puts("You Lose");
	}
	pause();
}

/*******************
**                **
**  Main Program  **
**                **
*******************/

int main(){
	initConsole();
	srand((unsigned)time(NULL));
	while(GameStarted){
		clrscr();
		Game();
	}
	return 0;
}

/**************
**           **
**  Library  **
**           **
**************/

void initConsole(void){
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hWnd = FindWindow("ConsoleWindowClass",NULL);
	SetConsoleTextAttribute(hOut,0x0F);
	char title[64];COORD cSize = {30,20};
	_snprintf(title,sizeof(title),"iBug Minesweeper %s",(const char*)Vers);
	SetConsoleTitle((LPCSTR)title);
	SetConsoleSize(cSize);
	CONSOLE_CURSOR_INFO CCI;GetConsoleCursorInfo(hOut,&CCI);
	CCI.bVisible = 0;SetConsoleCursorInfo(hOut,&CCI);
	DeleteMenu(GetSystemMenu(GetConsoleWindow(),0),SC_MAXIMIZE,MF_DISABLED);
}
void SetConsoleSize(COORD Size){
	SHORT bSizeX = Size.X*2;
	COORD bSize = {bSizeX<50?50:bSizeX , Size.Y+3};
	//Removed bSize.X++;
	SetConsoleScreenBufferSize(hOut,bSize);
	//No longer useful bSize.X = bSizeX;
	SMALL_RECT cSize = {0,0,bSize.X-1,bSize.Y-1};
	SetConsoleWindowInfo(hOut,1,&cSize);
}
inline void clrscr(void){
	COORD startPos = {0,0};
	FillConsoleOutputCharacter(hOut,0x20,16800,startPos,&clsCount);
	FillConsoleOutputAttribute(hOut,0x0F,16800,startPos,&clsCount);
	SetConsoleCursorPosition(hOut,startPos);
}
inline void pause(void){clearKeyBuf();while(!kbhit())Sleep(100);clearKeyBuf();}
inline void clearKeyBuf(void){while(kbhit())getch();}
COORD GetConsoleCursorPosition(HANDLE H){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(H,&CSBI);
	COORD Pos = CSBI.dwCursorPosition;
	return Pos;
}

int in_Mouse(COORD* Pos){
	static int i;static COORD pos;
	MEA |= MES;
	mousePosPrev = mousePos;
	MES = getMouseEvent(Pos, 7);
	if(MES<0)return (MES=0);
	//MEPR = MES;
	if(MES==0){ // Mouse released
		for(i=0;i<9;i++){
			pos.X = mousePos.X-1+i%3;pos.Y = mousePos.Y-1+i/3;
			printSingle(pos,0);
		}
		ME = MEA;
		MEA = 0;
		return ME;
	}else if(CoordCmp(mousePos,mousePosPrev)==0 || (MEA|MES)!=MEA){
		if((MEA|=MES)>=3){
			for(i=0;i<9;i++){
				//if(i==4)continue;
				pos.X = mousePosPrev.X-1+i%3;pos.Y = mousePosPrev.Y-1+i/3;
				printSingle(pos,0);
			}
			for(i=0;i<9;i++){
				pos.X = mousePos.X-1+i%3;pos.Y = mousePos.Y-1+i/3;
				printSingle(pos,2);
			}
		}else{
			printSingle(mousePosPrev,0);
			printSingle(mousePos,2);
		}
	}
	return 0;
}

long getMouseEvent(COORD* Pos,long btnStateReq){
	GetNumberOfConsoleInputEvents(hIn,(LPDWORD)&inC);
	if(inC==0) return -1;//If no input
	ReadConsoleInput(hIn, &inRec, 1, &readCount);
	if(inRec.EventType != MOUSE_EVENT)return -1;//Nothing with Mouse Happened
	*Pos = inRec.Event.MouseEvent.dwMousePosition;
	(Pos->Y)--;
	(Pos->X) /= 2;//Set Pos to Match Board
	if((Pos->X)>=boardX || (Pos->Y)>=boardY){*Pos=mousePosPrev;return -2;}//Invalid Position
	return btnStateReq & inRec.Event.MouseEvent.dwButtonState;
}

// Game Library

int getRealMines(COORD Pos){
	int x=Pos.X,y=Pos.Y,X=boardX-1,Y=boardY-1;
	if(board[x][y]==11)return -1;
	int Return = 0,i;COORD pos;
	for(i=0;i<9;i++){
		if(i==4)continue;
		pos.X = Pos.X-1+i%3;pos.Y = Pos.Y-1+i/3;
		if(isValidPos(pos))if(Board(pos)==10||Board(pos)==11)Return++;
	}
	Return = Return==0?9:Return;
	return Return; 
}
int getNonMine(COORD Pos){
	int x=Pos.X,y=Pos.Y,X=boardX-1,Y=boardY-1;
	if(board[x][y]==11)return -1;
	int Return = 0,i;COORD pos;
	for(i=0;i<9;i++){
		if(i==4)continue;
		pos.X = Pos.X-1+i%3;pos.Y = Pos.Y-1+i/3;
		if(isValidPos(pos)==0){Return++;continue;} 
		if(Board(pos)>=1&&Board(pos)<=9)Return++;
	}
	return Return; 
}
int getMarked(COORD Pos){
	int x=Pos.X,y=Pos.Y,X=boardX-1,Y=boardY-1;
	if(Board(Pos)==11)return -1;
	int Return = 0,i;COORD pos;
	for(i=0;i<9;i++){
		if(i==4)continue;
		pos.X = Pos.X-1+i%3;pos.Y = Pos.Y-1+i/3;
		if(isValidPos(pos))if(Board(pos)==10||Board(pos)==12)Return++;
	}
	return Return; 
}
void printSingle(COORD Pos,int Mode){
	if(!isValidPos(Pos))return;
	COORD cPos = {Pos.X*2,Pos.Y+1};
	SetConsoleCursorPosition(hOut,cPos);
	byte toPrint = (Board(Pos))&0x7F;
	switch(Mode){
		case 0:
		case 2: 
			if(toPrint==11)toPrint=0;
			if(toPrint==12)toPrint=10;
			break;
		case 1:
			if(toPrint==11)toPrint=10;
			break;
	}
	Color(imgColor[toPrint]);
	if(Mode==2 && toPrint!=10)Color(imgColor[9]);
	printf(img[toPrint]);
}
void printBoard(int Mode){
	int i,mxi=(unsigned int)(boardX*boardY),toPrint,toColor,mnCnt=mineCount;
	COORD CP = CCP;
	SetConsoleCursorPosition(hOut,CP);
	SetConsoleTextAttribute(hOut,0x1F);
	for(i=0;i<mxi;++i){
		toPrint = (board[i%boardX][i/boardX])&0x7F;
		switch(toPrint){
			case 10:case 12:
				--mnCnt;break;
		}
		if(Mode==0){
			if(toPrint==11)toPrint=0;
			if(toPrint==12)toPrint=10;
		}else if(win!=0 && toPrint==11){toPrint=10;--mnCnt;}
		if(Mode!=0&&win==0&&losePos.X==i%boardX&&losePos.Y==i/boardX){Color(0xEC);}
		else{Color(imgColor[toPrint]);}
		printf("%2s",(char*)img[toPrint]);
		if((i+1)%boardX==0 && (i+1)!=mxi){++CP.Y;SetConsoleCursorPosition(hOut,CP);}
	}
	SetConsoleCursorPosition(hOut,minePos);
	Color(0x0F);
	printf("%3d",mnCnt);
}
void Config(int param){for(;;){
	clrscr();int mk;
	puts("[SPACE] Previous Menu\n");
	puts("Settings: ");
	printf("\t[1] [%3s] Evading Mines\n",sw_tx[mycfg[0]]);
	printf("\t[2] [%3s] Generated Structure\n",sw_tx[mycfg[1]]);
	printf("\t[3] [%3s] Auto Fill Board\n\t",sw_tx[mycfg[2]]);Color(0x0C);
	printf("[4] [%3s] Silver Power Mode\n",sw_tx[mycfg[3]]);Color(0x0F);
	puts("Press a key to switch\n");
	clearKeyBuf();byte keyProc = 1;
	while(keyProc==1)switch(mk=getch()){
		case 0x31:case 0x32:case 0x33:case 0x34:
			mycfg[mk-49] = mycfg[mk-49]^0x01;
			keyProc = 0;break;
		case 0x20:return;
		case 0x00:case 0xE0:
			getch();break;
	}
}}
inline int isValidPos(COORD Pos){
	if(Pos.X<0 || Pos.X>=boardX || Pos.Y<0 || Pos.Y>=boardY)return 0;return 1;
}
inline int CoordCmp(COORD a,COORD b){
	if(a.X!=b.X || a.Y!=b.Y)return 0;
	return 1;
}

