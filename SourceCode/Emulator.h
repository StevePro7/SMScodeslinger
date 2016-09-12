#pragma once
#ifndef _SMS_EMULATOR_H
#define _SMS_EMULATOR_H

#include "Z80.h"
#include "TMS9918A.h"
#include "SN79489.h"

class Emulator
{
public:
	static	Emulator*					CreateInstance			( ) ;
	static	Emulator*					GetSingleton			( ) ;

									~Emulator				(void);

				void				Reset					( ) ;
				void				InsertCartridge			( const char* path ) ;
				void				Update					( ) ;

				 BYTE               ReadMemory              ( const WORD& address ) ;
				 void				WriteMemory				( const WORD& address, const BYTE& data ) ;
                 BYTE               ReadIOMemory            ( const BYTE& address ) ;
				 void				WriteIOMemory			( const BYTE& address, const BYTE& data ) ;
				 TMS9918A&			GetGraphicChip			( ) { return m_GraphicsChip; }

                 void               SetKeyPressed           (int player, int key);
                 void               SetKeyReleased          (int player, int key);
				 void				ResetButton				( ) ;
				 void				DumpClockInfo			( ) ;
				 void				SetGFXOpt				( bool useGFXOpt ) {m_GraphicsChip.SetGFXOpt(useGFXOpt) ;}
				 void				CheckInterupts			( ) ;


	static		 const	long long	MACHINE_CLICKS = 10738635;
	static		 const	int			CPU_CYCLES_TO_MACHINE_CLICKS = 3;

private:
	static		Emulator*			m_Instance ;
									Emulator				(void);

				bool				IsPAL					( ) const ;
				bool				IsCodeMasters			( ) ;
				void				DoMemPage				( WORD address, BYTE data ) ;
				void				DoMemPageCM				( WORD address, BYTE data ) ;

		
				unsigned long int	m_CyclesThisUpdate ;
				int					m_FPS ;
				TMS9918A			m_GraphicsChip ;
				SN79489				m_SoundChip ;

				Z80					m_Z80 ;

				BYTE				m_RamBank[0x2][0x4000] ;

                BYTE                m_KeyboardPort1;
                BYTE                m_KeyboardPort2;
				bool				m_IsPAL ;
				bool				m_IsCodeMasters ;
				bool				m_OneMegCartridge ;	
				unsigned long int	m_ClockInfo ;
				BYTE				m_FirstBankPage ;
				BYTE				m_SecondBankPage ;
				BYTE				m_ThirdBankPage ;
				int					m_CurrentRam ;
};

#endif
