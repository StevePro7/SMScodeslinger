#include <windows.h>
#include <shlobj.h>
#include <string>
#include "resource.h"
#include "Config.h"
#include "LogMessages.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	LogMessage* lm = LogMessage::CreateInstance();

	MessageBox (NULL, TEXT ("Hello, Windows 2016!"), TEXT ("HelloMsg"), 0) ;

	delete lm;
	return 0 ;
}