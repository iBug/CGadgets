#include <windows.h>
#include "main.h"

char szLastFileName[MAX_PATH];
BYTE bzKeyDown[256]; 

BOOL LoadFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		strcpy((LPSTR)szLastFileName,(LPCSTR)pszFileName); 
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF) {
			LPSTR pszFileText;
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL) {
				DWORD dwRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)) {
					pszFileText[dwFileSize] = 0; // Null terminator
					if(SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwTextLength;
		dwTextLength = GetWindowTextLength(hEdit);
		if(dwTextLength > 0) {
			LPSTR pszText;
			pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if(pszText != NULL) {
				if(GetWindowText(hEdit, pszText, dwTextLength + 1)) {
					DWORD dwWritten;
					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL DoFileOpenSave(HWND hwnd, DWORD dwMode) {
	OPENFILENAME ofn;
	char szFileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";

	if(dwMode == CM_FILE_SAVEAS) {
		ofn.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
		if(GetSaveFileName(&ofn)) {
			if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
		}
	}
	if(dwMode == CM_FILE_SAVE) {
		strcpy((LPSTR)szFileName,(LPCSTR)szLastFileName);
		if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
			MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}
	}
	if(dwMode == CM_FILE_OPEN){
		ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		if(GetOpenFileName(&ofn)) {
			if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Load of file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
			strcpy((LPSTR)szLastFileName,(LPCSTR)szFileName);
		}
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_CREATE:
			CreateWindow("EDIT", "",WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,hwnd, (HMENU)IDC_MAIN_TEXT, GetModuleHandle(NULL), NULL);
			SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
			break;
		case WM_SIZE:
			if(wParam != SIZE_MINIMIZED)
				MoveWindow(GetDlgItem(hwnd, IDC_MAIN_TEXT), 0, 0, LOWORD(lParam),HIWORD(lParam), TRUE);
			break;
		case WM_SETFOCUS:
			SetFocus(GetDlgItem(hwnd, IDC_MAIN_TEXT));
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case CM_FILE_OPEN:
				case CM_FILE_SAVE:
				case CM_FILE_SAVEAS:
					DoFileOpenSave(hwnd, LOWORD(wParam));
					break;
				case CM_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case CM_ABOUT:
					MessageBox (NULL, "iBug Text Editor for Windows!\nMaster XiangZe Plays Silver Power!" , "About", 0);
					break;
			}
			break;
		case WM_KEYDOWN:
			GetKeyboardState(bzKeyDown);
			if(bzKeyDown[VK_CONTROL] && bzKeyDown[(DWORD)'S'])
				DoFileOpenSave(hwnd, CM_FILE_SAVE);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize        = sizeof(wc);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = "MAINMENU";
	wc.lpszClassName = "WindowClass";
	wc.hIconSm       = LoadIcon(hInstance,"A"); /* A is name used by project icons */

	if(!RegisterClassEx(&wc)) {
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","iBug Text Editor",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,480,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(0, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	ShowWindow(hwnd,1);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
