#include "LogMessages.h"
#include "GameSettings.h"
#include "MasterSystem.h"

int main (void)
{
    LogMessage* lm = LogMessage::CreateInstance( );
    GameSettings* gs = GameSettings::CreateInstance( );
    gs->LoadSettings( );
    MasterSystem* sms = MasterSystem::CreateInstance( ) ;
    sms->CreateSDLWindow( ) ;
    sms->StartRom(gs->GetSetting("ROMName").c_str()) ;
    sms->BeginGame(atoi(gs->GetSetting("TargetFPS").c_str()));
    delete gs ;
    delete lm ;
    return 0;
}
