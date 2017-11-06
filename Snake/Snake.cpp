#include<iostream>
#include<cstdlib>
#include<conio.h>
#include<ctime>
#include<windows.h>
#define LEFT 3
#define RIGHT 1
#define UP 0
#define DOWN 2
#define map_x 25
#define map_y 20
#define MAX_LENGTH 200
#define BLOCK "¨€"
#define EMPTY "  "
#define SPEED 100
using namespace std;
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

class Cursor{
    public:
    void Hide(void){
        CONSOLE_CURSOR_INFO INFO;
        GetConsoleCursorInfo(hOut,&INFO);
        INFO.bVisible = false;
        SetConsoleCursorInfo(hOut,&INFO);
    }
};

extern "C" void _sleep(unsigned long);
/********************
* Pre-Configuration *
********************/

//class bigPoint{
//	public:
typedef struct{
    short x;
    short y;
    char color;
    string label;
    void Show(){
        COORD POS;
        POS.X = 2*x;
        POS.Y = y;
        SetConsoleCursorPosition(hOut,POS);
        SetConsoleTextAttribute(hOut,color);
        if(color==(char)0xC0 || color==(char)0xE0) SetConsoleTitle("Bang");
        cout<<label;
    }
    void Hide(){
        COORD POS;
        POS.X = 2*x;
        POS.Y = y;
        SetConsoleCursorPosition(hOut,POS);
        SetConsoleTextAttribute(hOut,0);
        cout<<EMPTY;
    }
} bigPoint;

void Home(){
    COORD RP = {0,0};
    SetConsoleCursorPosition(hOut,RP);
}
void prec();
/**********************
*Pre-Configuration End*
**********************/

const int map_round=2*(map_x+map_y)+5;
signed long int t;
unsigned long int active=4;
bool started=true;
bigPoint part[MAX_LENGTH];
bigPoint head;
bigPoint food;
bigPoint edge[map_round];

void Game(){
    prec();
    short dir=1;
    char ch;
    while(started){
        if(kbhit()){
            ch = getch();
            if(ch==(char)0xE0 || ch==0)ch=getch();
            switch(ch){
                case 119:case 87:
                    dir=UP;
                    break;
                case 115:case 83:
                    dir=DOWN;
                    break;
                case 97:case 65:
                    dir=LEFT;
                    break;
                case 100:case 68:
                    dir=RIGHT;
                    break;
            }
        }
        part[active-1].Hide();
        for(t=active-1;t>0;t--){
            part[t].x=part[t-1].x;
            part[t].y=part[t-1].y;
            part[t].Show();
        }
        part[0].x=head.x;
        part[0].y=head.y;
        part[0].Show();
        switch(dir){
            case UP:
                head.y--;
                break;
            case DOWN:
                head.y++;
                break;
            case LEFT:
                head.x--;
                break;
            case RIGHT:
                head.x++;
                break;
        }
        if(head.x<=0)head.x=map_x;
        if(head.x>map_x)head.x=1;
        if(head.y<=0)head.y=map_y;
        if(head.y>map_y)head.y=1;
        for(unsigned s=0;s<active;s++){
            if(head.x==part[s].x && head.y==part[s].y)started=false;
        }
        if(head.x==food.x && head.y==food.y){
            food.x=rand()%map_x+1;
            food.y=rand()%map_y+1;
            food.Show();
            if(active+1<MAX_LENGTH)active++;
        }
        head.Show();
        food.Show();
        _sleep(SPEED);
    }
    while(!kbhit());
}

void prec(){
    srand((unsigned)time(NULL));
    Cursor CursorObject;
    CursorObject.Hide();
    head.x=2;
    head.y=1;
    head.label=EMPTY;
    head.color=0x00CC;
    food.x=rand()%map_x+1;
    food.y=rand()%map_y+1;
    food.label=EMPTY;
    food.color=0x00EE;
    food.Show();
    for(t=0;t<MAX_LENGTH;t++){
        part[t].x=1;
        part[t].y=1;
        part[t].label=EMPTY;
        part[t].color=0x0030;
    }
    int tmp=0;
    for(t=0;t<map_x+1;t++){
        edge[tmp].x=t;
        edge[tmp].y=0;
        edge[tmp].label=EMPTY;
        edge[tmp].color=0x00F0;
        edge[tmp].Show();
        tmp++;
    }
    for(t=1;t<=map_x+1;t++){
        edge[tmp].x=t;
        edge[tmp].y=map_y+1;
        edge[tmp].label=EMPTY;
        edge[tmp].color=0x00F0;
        edge[tmp].Show();
        tmp++;
    }
    for(t=1;t<=map_y+1;t++){
        edge[tmp].x=0;
        edge[tmp].y=t;
        edge[tmp].label=EMPTY;
        edge[tmp].color=0x00F0;
        edge[tmp].Show();
        tmp++;
    }
    for(t=0;t<map_y+1;t++){
        edge[tmp].x=map_x+1;
        edge[tmp].y=t;
        edge[tmp].label=EMPTY;
        edge[tmp].color=0x00F0;
        edge[tmp].Show();
        tmp++;
    }
}

int main(){
	SetConsoleTitle("Snake by iBug - The Nothingville");
    SetConsoleTextAttribute(hOut,0xFC);
	Game();
	return 0;
}


