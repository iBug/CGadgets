// Originally made by Bill Matrin,
// This is a clone version by iBug
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define MoveCursor(Var) SetConsoleCursorPosition(hOut,(COORD)(Var))
const char Vers[32] = "Alpha 1.3.008 R1";

// System Vars & Lib Functions
typedef unsigned char byte;
HANDLE hOut;
int toLowerCase(char*),toUpperCase(char*),Error(byte);
byte bitByUp(byte*,int),bitByDown(byte*,int),bitBy(const byte,byte),isValidWord(char*);
COORD GetConsoleCursorPosition(HANDLE);
byte ErrorCode = 0;
void clearKeyBuf(void),clrscr(void),_sleep(unsigned);
char ErrorText[3][64] = {
	"Unknown! Issue!",
	"Create! Your! Dictionary! Here!",
	"All! Words! Are! Invalid!"
};

// Game Vars & Game Functions
char Word[32768][40];
char onWord[32],excd[26],prevWord[32]="No Word";
int wordCount,wordLen,wordChosen = -1,prevWordC,guessed;
int GameStarted,Winning = 0,Losing = 0;
COORD wordPos,excPos,lifePos;

void initGame(void){
	GameStarted = 1;
	prevWordC = wordChosen;
	do{wordChosen = rand()%wordCount;}
	while(wordChosen==prevWordC && wordCount>1);
	guessed = 0;
	strcpy((char*)onWord,(const char*)Word[wordChosen]);
	//toLowerCase(onWord);
	wordLen = strlen(onWord);
	int i;
	for(i=0;i<26;i++){excd[i]=0;bitByDown(&prevWord[i],7);}
	unsigned int winRatioI,winRatioD,allGames=Winning+Losing;
	if(allGames==0){winRatioI=0;winRatioD=0;}
	else{
		unsigned int winRT = (Winning*10000)/allGames;
		unsigned int winRS = winRT/10;
		if(winRT%10>4)winRS++;
		winRatioI = winRS/10;winRatioD = winRS%10;
	}
	printf("iBug WordGuess\nVersion %s\n\n",(const char*)Vers);
	printf("Games Won: %2d\nGames Lost: %3d\nWinning Ratio: %2d.%d%%\n",Winning,Losing,winRatioI,winRatioD);
	printf("Previous Word:\n\t%s\n\nWord:\n",prevWord);
	wordPos = GetConsoleCursorPosition(hOut);
	printf("\n\nAvailable:\n");
	excPos = GetConsoleCursorPosition(hOut);
	printf("\n\nLife: ");
	lifePos = GetConsoleCursorPosition(hOut);
}

void Main(void){
	clrscr();
	initGame();
	unsigned char keyIn,isMatch,chr;
	for(;GameStarted!=0;){
		isMatch = 0;keyIn = 0;
		//Print Word
		MoveCursor(wordPos);
		int i;
		for(i=0;i<wordLen;i++){
			chr = onWord[i];
			if(bitBy(chr,7)){
				printf("%c",chr&0x7F);
			}else printf("-");
		}
		//Print Excluded Char
		MoveCursor(excPos);
		for(i=0;i<26;i++)
			printf("%c",(char)(excd[i]==0?(0x41+i):0x20));
		//Print Lives Left
		MoveCursor(lifePos);
		printf("%2d",(7-guessed));
		//Read Next Guess
		while(keyIn<0x41 || (0x5A<keyIn && keyIn<0x61) || 0x7A<keyIn){ 
			keyIn = getch();
		}
		keyIn = keyIn & 0x5F;
		//Search if Letter Matches
		for(i=0;i<wordLen;++i){
			if((onWord[i]&0x5F)==keyIn){
				bitByUp(&onWord[i],7);isMatch = 1;
			}
		}
		if(!isMatch && excd[keyIn-0x41]==0)guessed++;
		excd[keyIn-0x41] = 1;
		//Check if Already Cleared
		isMatch = 1;
		for(i=0;i<wordLen;++i)if((onWord[i]&0x80)==0x00)isMatch=0;
		if(isMatch){
			GameStarted = 0;
			++Winning;
			strcpy(prevWord,onWord);
		}else if(guessed>7){
			GameStarted = 0;
			++Losing;
			strcpy(prevWord,onWord);
		}
	}
}

int initialize(void){
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut,0x0F); 
	char Title[64];
	snprintf(Title,sizeof(Title),"iBug WordGuess %s",Vers);
	SetConsoleTitle((LPCSTR)Title);
	CONSOLE_CURSOR_INFO CCI;
	GetConsoleCursorInfo(hOut,&CCI);
	CCI.bVisible = 0;
	SetConsoleCursorInfo(hOut,&CCI);
	srand((unsigned)time(NULL));
	FILE* Dict = fopen("Dict.txt","r");
	if(Dict==NULL){ErrorCode = 1;return 0;}
	int wdc;
	for(wdc=0;!feof(Dict);++wdc){
		fscanf(Dict,"%s",Word[wdc]);
		if(isValidWord(Word[wdc])!=0)wdc--;
	}
	if(wdc==0){ErrorCode = 2;return 0;}
	wordCount = wdc;
	fclose(Dict); 
	return 1;
}
// Ending Program
int main(void){if(initialize()==0){return Error(ErrorCode);}for(;;)Main();return 0;}


/*
    --------Libraries--------
*/
void clrscr(void){
	COORD startPos = {0,0};
	DWORD clsCount;
	FillConsoleOutputCharacter(hOut,0x20,16800,startPos,&clsCount);
	FillConsoleOutputAttribute(hOut,0x0F,16800,startPos,&clsCount);
	SetConsoleCursorPosition(hOut,startPos);
}
void pause(void){clearKeyBuf();while(!kbhit())_sleep(100);clearKeyBuf();}
void clearKeyBuf(void){while(kbhit())getch();}
COORD GetConsoleCursorPosition(HANDLE H){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	GetConsoleScreenBufferInfo(H,&CSBI);
	return CSBI.dwCursorPosition;
}
int Error(byte ec){
	FILE* errFile = fopen("Dict.txt","w");
	fprintf(errFile,ErrorText[ec]);
	return -1;
}
int toLowerCase(char* arg0){
	int var0;
	char *var1 = arg0;
	for(var0=0;var0>=0;var0++){
		if((unsigned char)var1[var0]<0x20)return var0;
		if(0x40<var1[var0]&&var1[var0]<0x5B)
			var1[var0]+=0x20;
	}
	return -1;
}
int toUpperCase(char* arg0){
	int var0;
	char* var1 = arg0;
	for(var0=0;var0>=0;var0++){
		if((unsigned char)var1[var0]<0x20)return var0;
		if(0x60<var1[var0]&&var1[var0]<0x7B)
			var1[var0]-=0x20;
	}
	return -1;
}
byte bitByUp(byte* arg0,int arg1){
	byte var0 = (*arg0);
	var0 = var0|(1<<arg1);
	return (*arg0=var0);
}
byte bitByDown(byte* arg0,int arg1){
	byte var0 = (*arg0);
	var0 = var0&(~(1<<arg1));
	return (*arg0=var0);
}
byte bitBy(const byte arg0,byte arg1){if(arg1>=8)return -1;return((arg0&((byte)(1<<arg1)))!=0);}
byte isValidWord(char* Word){
	int i,k,allLine = 1;
	if(Word[0]==0)return 1;//Empty Word
	for(i=0;i<40;i++){
		if(Word[i]==0)return allLine;
		if(Word[i]=='_'){
			Word[i]=0xA0;
			continue;
		}
		//When a Preserved Char Comes 
		if(Word[i]=='\\'){
			Word[i] = Word[i+1] | 0x80;
			for(k = i+1;Word[k]!=0;k++){Word[k] = Word[k+1];}
			Word[k] = 0x0;
			continue;
		}
		if((Word[i]<0x41 || (0x5A<Word[i] && Word[i]<0x61) || 0x7A<Word[i]))return 1;
		allLine = 0;
	}
	return 2;//Too Long
}
