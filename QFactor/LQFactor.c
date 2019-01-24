#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#define HOUT hOut

typedef unsigned long long int ul;
ul n,ni,f,i,q,fl,fc[1024],fcc[1024],err;
HANDLE HOUT;

void clrscr(void){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	COORD a = {0,0};DWORD z;
	GetConsoleScreenBufferInfo(HOUT,&CSBI);
	FillConsoleOutputCharacter(HOUT,(CHAR)0,CSBI.dwSize.X*CSBI.dwSize.Y,a,&z);
	FillConsoleOutputAttribute(HOUT,(WORD)7,CSBI.dwSize.X*CSBI.dwSize.Y,a,&z);
	SetConsoleCursorPosition(HOUT,a);
}
int main(){
	HOUT = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(HOUT,0x0F);
	starting:
	clrscr();
	if(err)printf("\n  iBug QuickFactoring\n\nDon't play silver power!\nEnter a number: ");
	else printf("\n  iBug QuickFactoring\n\nEnter a number: ");
	if(scanf("%I64u",&n)==0){
		gets((char*)fc);
		err = 1;
		goto starting;
	}
	err = 0;
	ni = n;f = 1;fl = 0;
	for(i=2;i<=(ul)sqrt(n)+1;i++){
		if(n%i==0){
			fcc[fl] = f = 0;
			fc[fl] = i;
			while(n%i==0){
				n /= i;
				fcc[fl]++;
			}
			fl++;
		}
	}
	if(f==1)puts("Is Prime\n");
	else{
		if(fcc[0]>1)printf("%I64u = %I64u^%I64u",ni,fc[0],fcc[0]);
		else if(fcc[0]==1)printf("%I64u = %I64u",ni,fc[0]);
		for(i=1;i<fl;i++){
			if(fcc[i]>1)printf(" * %I64u^%I64u",fc[i],fcc[i]);
			if(fcc[i]==1)printf(" * %I64u",fc[i]);
		}
		if(n>1)printf(" * %I64u",n);
	} 
	puts("");
	while(kbhit())getch();
	while(!kbhit());
	while(kbhit())getch();
	goto starting;
}
