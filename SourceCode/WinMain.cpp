#include <windows.h>

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     MessageBox (NULL, TEXT ("Hello, Windows 2016!"), TEXT ("HelloMsg"), 0) ;

     return 0 ;
}