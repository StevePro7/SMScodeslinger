#include "Config.h"
#include <windows.h>
#include "resource.h"
#include "LogMessages.h"
#include "MasterSystem.h"
#include <shlobj.h>
#include <string>
#include "Z80.Mnemonics.h"
//////////////////////////////////////////////////////////////////////////////////////

#define MENU_FILE_LOAD_ROM 9000
#define MENU_FILE_LOAD_DIR 9001
#define MENU_FILE_LOAD_STATE 9002
#define MENU_FILE_SAVE_STATE 9003
#define MENU_FILE_EXIT 9004
#define MENU_EDIT_CONFIGURE 9005
#define MENU_EMU_RESUME 9006
#define MENU_EMU_PAUSE 9007
#define MENU_EMU_STOP 9008
#define MENU_EMU_RESET 9009
#define MENU_EMU_DEBUGGER 9010


// globals... how immensely embarrassing
static HWND g_Hwnd = 0 ;
static HWND g_Debugger = 0 ;
static HMENU g_FileSub = 0 ;
static HMENU g_EditSub = 0 ;
static HMENU g_EmuSub = 0 ;
static MasterSystem* g_MasterSystem = 0 ;

//////////////////////////////////////////////////////////////////////////////////////

void LoadDebugInfo( )
{
	for (int i = 0 ; i < 0xC000; i++)
	{
		BYTE byte = g_MasterSystem->GetMemoryByte(i) ;
		char buffer[256] ;
		WORD index = byte ;
		BYTE newByte ;

		if (byte == 0xCB || byte == 0xDD || byte == 0xED || byte == 0xFD)
		{
			switch (byte)
			{
			case 0xCB: newByte = g_MasterSystem->GetMemoryByte(i) ; break ;
			}

		}
		else
		{		
			sprintf(buffer, "%x:    %s", byte, Z80MNEMONICSSTANDARD[byte]) ;
			SendDlgItemMessage(g_Debugger, IDC_MEM, LB_ADDSTRING, 0, (LPARAM)buffer);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void StartRom (const char* path)
{
	g_MasterSystem->CreateSDLWindow( ) ;
	g_MasterSystem->StartRom(path) ;
	//LoadDebugInfo( ) ;
	g_MasterSystem->BeginGame(60,false) ;
}

//////////////////////////////////////////////////////////////////////////////////////

void SetMenuState( bool inGame )
{
	int enabled = MF_STRING | MF_ENABLED;
	int disabled = MF_GRAYED | MF_DISABLED ;

	ModifyMenu(g_FileSub, MENU_FILE_SAVE_STATE, inGame?enabled:disabled, MENU_FILE_SAVE_STATE, "Save State" );
	ModifyMenu(g_EmuSub, MENU_EMU_RESUME,inGame?enabled:disabled, MENU_EMU_RESUME, "Resume" );
	ModifyMenu(g_EmuSub, MENU_EMU_PAUSE, inGame?enabled:disabled, MENU_EMU_PAUSE, "Pause" );
	ModifyMenu(g_EmuSub, MENU_EMU_STOP, inGame?enabled:disabled, MENU_EMU_STOP, "Stop" );
	ModifyMenu(g_EmuSub, MENU_EMU_RESET, inGame?enabled:disabled, MENU_EMU_RESET, "Reset" );
	ModifyMenu(g_EmuSub, MENU_EMU_DEBUGGER, inGame?enabled:enabled, MENU_EMU_DEBUGGER, "Debugger" );

	ModifyMenu(g_FileSub, MENU_FILE_LOAD_ROM, inGame?disabled:enabled, MENU_FILE_LOAD_ROM, "Load Rom" );
	ModifyMenu(g_FileSub, MENU_FILE_LOAD_DIR, inGame?disabled:enabled, MENU_FILE_LOAD_DIR, "Load Rom Directory" );
	ModifyMenu(g_FileSub, MENU_FILE_LOAD_STATE, inGame?disabled:enabled, MENU_FILE_LOAD_STATE, "Load State" );
	ModifyMenu(g_EditSub, MENU_EDIT_CONFIGURE,inGame?disabled:enabled, MENU_EDIT_CONFIGURE, "Configure") ;
}

//////////////////////////////////////////////////////////////////////////////////////

void ResetBeginGameState( )
{
	EnableWindow(GetDlgItem(g_Hwnd, IDC_ROMLIST),true) ;
	EnableWindow(GetDlgItem(g_Hwnd, IDSTART),true) ;
	SetMenuState(false) ;	
}

//////////////////////////////////////////////////////////////////////////////////////

void SetBeginGameState( )
{
	EnableWindow(GetDlgItem(g_Hwnd, IDC_ROMLIST),false) ;
	EnableWindow(GetDlgItem(g_Hwnd, IDSTART),false) ;
	SetMenuState(true) ;	
}

//////////////////////////////////////////////////////////////////////////////////////

void StartEmulationFromFile(const char* filename)
{
	SetBeginGameState( ) ;
	StartRom(filename) ;
}

//////////////////////////////////////////////////////////////////////////////////////

void StartEmulationFromList( )
{
	// get the associative list box entry for the current hightlighter
	int index = SendMessage(GetDlgItem(g_Hwnd,IDC_ROMLIST), LB_GETCURSEL,0,0) ;
	int length = SendMessage(GetDlgItem(g_Hwnd,IDC_ROMLIST), LB_GETTEXTLEN, (WPARAM)index, 0);
	std::string path ;

	char* buf ;
	buf = (char*)GlobalAlloc(GPTR, length + 1);

	SendMessage(GetDlgItem(g_Hwnd,IDC_ROMLIST), LB_GETTEXT, (WPARAM)index, (LPARAM)buf);
	path = buf ;
	GlobalFree((HANDLE)buf);

	SetBeginGameState( ) ;
	StartRom(path.c_str()) ;

}

//////////////////////////////////////////////////////////////////////////////////////

void PopulateRomList(std::string path)
{
	WIN32_FIND_DATA ffd;
	path += "\\" ;
	std::string fullpath = path ;

	fullpath += "*" ;
	HANDLE hFind = FindFirstFile(fullpath.c_str(), &ffd);

	do
	{
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)== false)
		{
			std::string fullpath = path + ffd.cFileName ;
			int stringlength = fullpath.size() ;
			if (stringlength > 2)
			{
				for (int i = 1 ; i <= 3; i++)
				{
					fullpath.at(stringlength-i) = tolower(fullpath.at(stringlength-i)) ;
				}
			}

			// make sure its got the correct extension
			if (strstr(fullpath.c_str(),".sms") != 0)
			{
				int index = SendDlgItemMessage(g_Hwnd, IDC_ROMLIST, LB_ADDSTRING, 0, (LPARAM)fullpath.c_str() );	
			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);
}

//////////////////////////////////////////////////////////////////////////////////////

void DoLoadRomFile( )
{
	OPENFILENAME ofn;
	char buf[260];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_Hwnd;
	ofn.lpstrFile = buf;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(buf);
	ofn.lpstrFilter = "Master System\0*.sms\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) != 0)
	{
		StartEmulationFromFile(buf) ;
		ResetBeginGameState( ) ;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void DoLoadRomDir( )
{
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = "Select your Rom directory";
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		CHAR path[MAX_PATH];
		if ( SHGetPathFromIDList ( pidl, path ) )
		{
			PopulateRomList( path ) ;
		}

		// free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DebugProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE) ;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE ;
}

//////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MainProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_CLOSE:
		DestroyWindow(g_Debugger);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case MENU_EMU_DEBUGGER:
			ShowWindow(g_Debugger, SW_SHOW) ;
			break ;
		case MENU_FILE_EXIT:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case MENU_FILE_LOAD_ROM:
			DoLoadRomFile( ) ;
			break;
		case MENU_FILE_LOAD_DIR:
			DoLoadRomDir( ) ;
			break;
		case IDSTART:
			StartEmulationFromList( ) ;
			ResetBeginGameState( ) ;
			break ;
		case IDCLOSE:
			DestroyWindow(hwnd);
			break ;
		case IDC_ROMLIST:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				EnableWindow(GetDlgItem(g_Hwnd,IDSTART),true) ;
				break;
			}
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

bool MenuCreate( )
{
	HMENU hMenu ;

	hMenu = CreateMenu();

	g_FileSub = CreatePopupMenu();
	AppendMenu(g_FileSub, MF_STRING, MENU_FILE_LOAD_ROM, "Load Rom");
	AppendMenu(g_FileSub, MF_STRING, MENU_FILE_LOAD_DIR, "Load Rom Directory");
	AppendMenu(g_FileSub, MF_SEPARATOR, 0, NULL) ;
	AppendMenu(g_FileSub, MF_STRING, MENU_FILE_LOAD_STATE, "Load State");
	AppendMenu(g_FileSub, MF_STRING, MENU_FILE_SAVE_STATE, "Save State");
	AppendMenu(g_FileSub, MF_SEPARATOR, 0, NULL) ;
	AppendMenu(g_FileSub, MF_STRING, MENU_FILE_EXIT, "E&xit");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)g_FileSub, "&File");

	g_EditSub = CreatePopupMenu();
	AppendMenu(g_EditSub, MF_STRING, MENU_EDIT_CONFIGURE, "Configure");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)g_EditSub, "&Edit");

	g_EmuSub = CreatePopupMenu();
	AppendMenu(g_EmuSub, MF_STRING, MENU_EMU_RESUME, "Resume");
	AppendMenu(g_EmuSub, MF_STRING, MENU_EMU_PAUSE, "Pause");
	AppendMenu(g_EmuSub, MF_STRING, MENU_EMU_STOP, "Stop");
	AppendMenu(g_EmuSub, MF_STRING, MENU_EMU_RESET, "Reset");
	AppendMenu(g_EmuSub, MF_STRING, MENU_EMU_DEBUGGER, "Debugger");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)g_EmuSub, "&Emulator");

	SetMenu(g_Hwnd, hMenu);

	SetMenuState(false) ;


	return true ;
}

//////////////////////////////////////////////////////////////////////////////////////

bool WindowCreate(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "windowclass";
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		return false;
	}

	// Step 2: Creating the Window
	g_Hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DIALOG_MAIN),NULL, MainProc);

	ShowWindow(g_Hwnd, SW_SHOW);
	UpdateWindow(g_Hwnd);

	g_Debugger = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DIALOG_DEBUG),NULL, DebugProc);
	ShowWindow(g_Debugger, SW_HIDE);
	UpdateWindow(g_Debugger);

	return true ;
}

//////////////////////////////////////////////////////////////////////////////////////

void MainWindowLoop( )
{
	MSG msg ;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LogMessage* lm = LogMessage::CreateInstance( ) ;

	bool success = true;
	//bool success = WindowCreate(hInstance) ;

	if (false == success)
	{
		lm->DoLogMessage("Error creating the window", false) ;
		delete lm ;
		return -1 ;
	}

	//success = MenuCreate( ) ;

	if (false == success)
	{
		lm->DoLogMessage("Error creating the menu", false) ;
		delete lm ;
		return -1 ;
	}

	g_MasterSystem = MasterSystem::CreateInstance( ) ;



	//MainWindowLoop( ) ;
	//char buf[260] = "C:\\Users\\TEST\\Downloads\\SMS-Slinger_Source\\SMS-Slinger_Source\\SMS-Slinger_Source\\ROMS\\Flicky.sg";
	char buf[260] = "ROMS\\Flicky.sg";
	StartEmulationFromFile(buf) ;
	ResetBeginGameState( );

	delete g_MasterSystem ;
	delete lm ;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

