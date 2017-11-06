
// This is only an addition of "Starbound"
// Should be copied to "Starbound.c" by "#include"
// Since this file includes seldom altered functions

typedef unsigned char byte;
typedef struct _COORD COORD;
// Fall
byte Fall(byte x){
	if(x>=boardW)return 0xFF;
	byte i,r=0,s=0;
	if(board[x][0]==0){
		board[x][0] = newItem();
		data[x][0] |= 0x01;
		s = 1;r++;
	}
	for(i=1;i<boardH;i++){
		if(board[x][i]==0){
			Swap(&board[x][i],&board[x][i-1]);
			data[x][i] |= 0x01;data[x][i-1] |= 0x01;
			if(s==0){s=1;r++;}
		}
		else s = 0;
	}
	return r;
}

byte Horizontal(COORD Pos)// Horizontal Combo
{
	if(!isValidPos(Pos))return 0;
	const int t = Board(Pos);
	int i,o,l;
	for(i=Pos.X;;i--){
		if(i<0)break;
		if(board[i][Pos.Y]!=t)break;
	}
	o = Pos.X-(++i);
	for(i=Pos.X-o;;i++){
		if(i>=boardW)break;
		if(board[i][Pos.Y]!=t)break;
	}
	l = (--i)-(Pos.X-o);
	return (o|(l<<4))&0xFF;
}
byte Vertical(COORD Pos)// Vertical Combo
{
	if(!isValidPos(Pos))return 0;
	const int t = Board(Pos);
	int i,o,l;
	for(i=Pos.Y;;i--){
		if(i<0)break;
		if(board[Pos.X][i]!=t)break;
	}
	o = Pos.Y-(++i);
	for(i=Pos.Y-o;;i++){
		if(i>=boardH)break;
		if(board[Pos.X][i]!=t)break;
	}
	l = (--i)-(Pos.Y-o);
	return (o|(l<<4))&0xFF;
}

inline byte newItem(){
	int r = random;
	return r>32000?0xF0+random%ASO:random%AO;
}

int isValidPos(COORD Pos){
	if(Pos.X<0 || Pos.Y<0 || Pos.X>=boardW || Pos.Y>=boardH)
	return 0;return 1;
}

inline int printBoard(int Mode){
	// For Functions with "Mode" see printSingle
	int i,r = 0;COORD Pos;
	for(i=0;i<boardS;i++){
		Pos.X = i%boardW;Pos.Y = i/boardW;
		r += printSingle(Pos,Mode);
	}
	GetConsoleCursorPosition(hO,&Pos);
	SetConsoleCursorPosition(hO,scorePos);
	printf("%8d",Game.score);
	SetConsoleCursorPosition(hO,Pos);
	return r;
}

inline int printSingle(COORD Pos,int Mode){
	/*
	  Var "Mode":
		Bit 0x00000001 - "Selected"
	*/
	COORD printPos = COORDplus(Pos,boardPos);
	SetConsoleTextAttribute(hO,element[Board(Pos)].color);
	if(Mode&0x01){posOut(printPos,element[Board(Pos)].iconSelected);}
	else{posOut(printPos,element[Board(Pos)].icon);}
	SetConsoleTextAttribute(hO,0x0F);
	return 0;
}

#ifdef Comment
### Seperator
#endif 

inline COORD COORDplus(COORD A,COORD B)
{COORD R = {(A).X+(B).X,(A).Y+(B).Y};return R;}
inline COORD COORDminus(COORD A,COORD B)
{COORD R = {(A).X-(B).X,(A).Y-(B).Y};return R;}

