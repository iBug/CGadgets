#include <stdio.h>
#include <time.h>
#include <windows.h>
#undef NULL
#define NULL ((void*)0)
#define TICK_L 2
#define hOut hO
#define hIn hI

#define Board(A) (board[(A).X][(A).Y])
#define Data(A) (data[(A).X][(A).Y])
#define random (rand())
#define boardW 12
#define boardH 20
#define boardS (boardW*boardH)
#define AO 7
#define ASO 6 
const char Vers[32] = "InDev Alpha 1.0.001";

// General Library
typedef unsigned char byte;
void Swap(byte* a,byte* b){byte t=*a;*a=*b;*b=t;}
void initConsole(),Tick();
void posOut(COORD,const char*),posOutX(COORD,const char*);
inline void _sleep(unsigned long);
int GetMouseEvent(COORD*);
long getLastMouseEvent(COORD*,long),ME,MES,MEPR;
unsigned long clrscr();
COORD COORDplus(COORD,COORD),COORDminus(COORD,COORD),GetConsoleCursorPosition();
HANDLE hO,hI;INPUT_RECORD inRec;
DWORD inC,readCount;

// Game Data
//   -> Constants
const char icon[16][3] = {"  ","¡ï","¡î","¡ñ","¡ð","¡ô","¡ó","¡ö","¡õ","¡ø","¡÷","¡ò","©I","Ë£"};
const unsigned short backColor[7] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06};
const unsigned short frontColor[7] = {0x90,0xA0,0xB0,0xC0,0xD0,0xE0,0xF0};

//   -> Types & Vars
//   -> IMPORTANT! Left = -x, Right = +x, Up = -y, Down = +y
typedef struct _Element{char icon[4];char iconSelected[4];byte color;} Element;
const Element element[256] = {{"  ","  ",0x0F},
	{"¡î","¡ï",0x1F},{"¡ð","¡ñ",0x2F},{"¡ó","¡ô",0x3F},
	{"¡õ","¡ö",0x4F},{"¡÷","¡ø",0x5F},{"¨Œ","¨‹",0x6F},{"¨‘","¨’",0x8F},
	[0xF0]={"©I","©I",0x0F},[0xF1]={"¡ü","¡ü",0x0F},[0xF2]={"¡ý","¡ý",0x0F},
	[0xF3]={"¡û","¡û",0x0F},[0xF4]={"¡ú","¡ú",0x0F},[0xF5]={"¡­","¡­",0x0F} 
};
struct{int started;int ended;int won;long score;}Game={1,0,0};
byte board[boardW][boardH],data[boardW][boardH];
int isAltered(COORD p){return Data(p)&0x01;}
int trigger(COORD,int),printSingle(COORD,int),printBoard(int),isValidPos(COORD);
COORD mousePos,selected,boardPos = {1,2},scorePos;
const COORD zPos = {0,0};

//#define Tick(a)
// The Core of the Program: Game
//   -> Functions
//   Some of these is defined in "Ops.c", see bottom
int initGame(),trigger(COORD,int),check();
byte Vertical(COORD),Horizontal(COORD),newItem();
/*
 Check for 3-combos, return:
 The bits (0xF0) is "length-1", the bits (0x0F) is "Offset"
 Offset means how far is it(input) from starting of the combo, counting to Left/Up
 If offset is zero the input is the start of the combo
*/
byte Fall(byte);// To fill the blanks by falling, input column, return "Hole" count
// For example, 11110010001111 Returns "Holes" as 2 instead of 5

int initGame(){
	Game.started = 1;Game.ended = 0;Game.won = 0;Game.score = 0;
	clrscr();
	printf("Score: ");scorePos = GetConsoleCursorPosition();
	printf("%8d",Game.score);
	// Render the edge
	int i;COORD Pos;
	SetConsoleTextAttribute(hO,0xF0);
	for(i=-1;i<=boardW;i++){
		Pos.X = boardPos.X+i;Pos.Y = boardPos.Y-1;posOut(Pos,"  ");
		Pos.X = boardPos.X+i;Pos.Y = boardPos.Y+boardH;posOut(Pos,"  ");
	}
	for(i=-1;i<=boardH;i++){
		Pos.Y = boardPos.Y+i;Pos.X = boardPos.X-1;posOut(Pos,"  ");
		Pos.Y = boardPos.Y+i;Pos.X = boardPos.X+boardW;posOut(Pos,"  ");
	}
	// Up to now Nothing than Board Generation
	srand((unsigned long)time(NULL));
	for(i=0;i<boardS;i++){
		board[i%boardW][i/boardW] = newItem();// Generate a board
		data[i%boardW][i/boardW] = 1;// Mark the whole board as "Altered"
	}
	printBoard(0);
	while(check()!=0)Tick();
}

int check(){
	int r = 0,f = 0,i;
	for(i=0;i<boardW;i++)f+=Fall(i);
	if(f>0){printBoard(0);return f;}COORD Pos;int scn;
	for(i=boardS-1;i>=0;i--){
		if(data[i%boardW][i/boardW]&0x01==1){
			Pos.X = i%boardW;Pos.Y = i/boardW;
			Data(Pos) &= 0xFE;
			scn = trigger(Pos,0);
			Game.score += (scn>=0?10*scn:0);
			if(scn>0)r++;
			continue;
		}
	}
	printBoard(0);
	return r;// "r": Any alternation in the whole board?
}

// Here comes the core of the program: Function "Trigger"
// Returns Processed Objects Number if an item is given
// Returns (Negative)Fallen Holes if a hole is sent to argument "Pos"
int trigger(COORD Pos,int Mode){
	if(Board(Pos)==0)return 0;//-Fall(Pos.X);
	if(Board(Pos)>=0xF0 && Mode==1){
		COORD pos;int t=Board(Pos);Board(Pos)=0;
		switch(t){
			case 0xF0:
				for(int i=0;i<9;i++){
					pos.X=Pos.X+i%3-1;pos.Y=Pos.Y+i/3-1;
					if(!isValidPos(pos))continue;
					if(Board(pos)>=0xF0)trigger(pos,Mode);
					Board(pos)=0;Data(pos)|=0x01;
				}break;
			case 0xF1:
				for(int i=Pos.Y;i>=0;i--){
					if(board[Pos.X][i]>=0xF0){
						pos.X=Pos.X;pos.Y=i;
						trigger(pos,Mode);
					}
					board[Pos.X][i]=0;
					data[Pos.X][i]|=0x01;
				}break;
			case 0xF2:
				for(int i=Pos.Y;i<boardH;i++){
					if(board[Pos.X][i]>=0xF0){
						pos.X=Pos.X;pos.Y=i;
						trigger(pos,Mode);
					}
					board[Pos.X][i]=0;
					data[Pos.X][i]|=0x01;
				}break;
			case 0xF3:
				for(int i=Pos.X;i>=0;i--){
					if(board[i][Pos.Y]>=0xF0){
						pos.X=i;pos.Y=Pos.Y;
						trigger(pos,Mode);
					}
					board[i][Pos.Y]=0;
					data[i][Pos.Y]|=0x01;
				}break;
			case 0xF4:
				for(int i=Pos.X;i<boardW;i++){
					if(board[i][Pos.Y]>=0xF0){
						pos.X=i;pos.Y=Pos.Y;
						trigger(pos,Mode);
					}
					board[i][Pos.Y]=0;
					data[i][Pos.Y]|=0x01;
				}break;
			case 0xF5:
				for(int i=0;i<boardW;i++){
					if(board[i][Pos.Y]>=0xF0){
						pos.X=i;pos.Y=Pos.Y;
						trigger(pos,Mode);
					}
					board[i][Pos.Y]=0;
					data[i][Pos.Y]|=0x01;
				}break;
		}
		return 0;
	}
	byte HC,HO,HL,VC,VO,VL;int rtn = 0;
	// Words: H=Horizontal V=Vertical L=Length O=Offset C=Combo
	HC = Horizontal(Pos);HO = HC&0x0F;HL = HC/16;
	VC = Vertical(Pos);VO = VC&0x0F;VL = VC/16;
	if(HL<2 && VL<2)return 0;// No combo here
	int i;
	if(HL>1){
		for(i=Pos.X-HO;i<=Pos.X-HO+HL;i++){
			board[i][Pos.Y] = 0; 
			data[i][Pos.Y] |= 0x01;// Mark Altered
			rtn++;
		}
	}
	if(VL>1){
		for(i=Pos.Y-VO;i<=Pos.Y-VO+VL;i++){
			board[Pos.X][i] = 0;
			data[Pos.X][i] |= 0x01;
			rtn++;
		}
	}
	printSingle(Pos,0);
	return rtn;
}

int Play(){
	initGame();
	int mouse,dx,dy,act;COORD Pos;
	while(!Game.ended){
		// Read a mouse input
		mouse = GetMouseEvent(&mousePos);
		if(mouse==0)continue;
		// If clicked outside the board
		Pos = COORDminus(mousePos,boardPos);
		if(!isValidPos(Pos))continue;
		// Then select it
		if(Board(Pos)>=0xF0){
			trigger(selected=Pos,1);
		}
		else{
			if(selected.X<0 || selected.Y<0){selected = Pos;printSingle(Pos,1);continue;}
			// Here it's already selected
			dx = selected.X-Pos.X;dy = selected.Y-Pos.Y;
			if(dx==0 && dy==0){selected.X=-1;selected.Y=-1;printSingle(Pos,0);continue;}
			// Check if adjacent
			if(dx<-1||dx>1||dy<-1||dy>1||dx*dy!=0){printSingle(selected,0);selected.X=-1;selected.Y=-1;continue;}
			// Proceed!
			Swap(&Board(selected),&Board(Pos));
		}
		Data(selected) |= 0x01;Data(Pos) |= 0x01;
		act = check();
		if(act==0){
			Swap(&Board(selected),&Board(Pos));
			printSingle(selected,0);printSingle(Pos,0);
			selected.X=-1;selected.Y=-1;continue;
		}// Nothing after swap
		while(act){Tick();act = check();}
		selected.X=-1;selected.Y=-1;
	}
}

// Main Function - Only 1 line, believe?
int main(){initConsole();while(Game.started)Play();return 0;}

void initConsole(void){
	hO = GetStdHandle(STD_OUTPUT_HANDLE);
	hI = GetStdHandle(STD_INPUT_HANDLE);
	COORD bSize = {80,25};
	SetConsoleScreenBufferSize(hOut,bSize);
	SMALL_RECT cSize = {0,0,bSize.X-1,bSize.Y-1};
	SetConsoleWindowInfo(hOut,1,&cSize);
	SetConsoleTextAttribute(hO,0x0F);
	char title[64];
	snprintf(title,sizeof(title),"iBug Starbound %s",Vers);
	SetConsoleTitle((LPCSTR)title);
	CONSOLE_CURSOR_INFO CCI;GetConsoleCursorInfo(hO,&CCI);
	CCI.bVisible = 0;SetConsoleCursorInfo(hO,&CCI);
}
COORD GetConsoleCursorPosition(){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(hO,&CSBI);
	return CSBI.dwCursorPosition;
}
unsigned long clrscr(void){
	unsigned long O;COORD cPos = {0,0};
	FillConsoleOutputAttribute(hO,0x0F,1536,cPos,&O);
	FillConsoleOutputCharacter(hO,0x00,1536,cPos,&O);
	SetConsoleCursorPosition(hO,cPos);
	return O;
}
inline void posOut(COORD p,const char* s){p.X *= 2;posOutX(p,s);}
void posOutX(COORD p,const char* s){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(hO,&CSBI);
	SetConsoleCursorPosition(hO,p);printf(s);
	SetConsoleCursorPosition(hO,CSBI.dwCursorPosition);
}
int GetMouseEvent(COORD* Pos){
	MEPR = MEPR|MES;
	MES = getLastMouseEvent(Pos,0x03);
	if(MES==-1)return MES=0;
	if(MES==0){
		ME = MEPR;
		MEPR = 0;
		return ME;
	}else return 0;
}
long getLastMouseEvent(COORD* Pos,long btnStateReq){
	GetNumberOfConsoleInputEvents(hIn,(LPDWORD)&inC);
	if(inC==0)return -1;//If no input
	for(;inC>1;inC--)ReadConsoleInput(hIn,&inRec,1,&readCount);
	ReadConsoleInput(hIn,&inRec,1,&readCount);
	if(inRec.EventType != MOUSE_EVENT)return -1;//Nothing Mouse-ic Happened
	(*Pos) = inRec.Event.MouseEvent.dwMousePosition;
	Pos->X /= 2;
	long btnState = inRec.Event.MouseEvent.dwButtonState;
	long Return = btnState & btnStateReq;
	return Return;
}
#undef Tick
inline void Tick(){_sleep(TICK_L);}

/****************/
/* Game Library */
/****************/


// Copy a file
#include "Ops.c"

