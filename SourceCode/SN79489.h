#pragma once
#ifndef _SN79489_H
#define _SN79489_H

#ifdef WIN32
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif

class SN79489
{
public:
								SN79489					(void);
								~SN79489				(void);

			void				WriteData				( unsigned long int cycles, BYTE data ) ;
			void				Reset					( ) ;
			void				Update					( float cycles ) ;
			void				DumpClockInfo			( ) ;
private:

	enum CHANNEL
	{
		CHANNEL_ZERO,
		CHANNEL_ONE,
		CHANNEL_TWO,
		CHANNEL_THREE,
		CHANNEL_NUM
	} ;

	enum TONES
	{
		TONES_ZERO,
		TONES_ONE,
		TONES_TWO,
		TONES_NOISE,
		TONES_NUM
	};

	enum VOLUME
	{
		VOLUME_ZERO,
		VOLUME_ONE,
		VOLUME_TWO,
		VOLUME_THREE,
		VOLUME_NUM
	};
	
	static	void				HandleSDLCallback		( void* userData, Uint8* buffer, int len );
			void				HandleSDLCallback		( Uint8* buffer, int len ) ;
			void				OpenSDLAudioDevice		( ) ;

	static	const unsigned int	BUFFERSIZE = 1024 ;
	static	const int			FREQUENCY = 44100;
			signed short int	m_Buffer[BUFFERSIZE] ;
			WORD				m_Tones[TONES_NUM] ;
			BYTE				m_Volume[VOLUME_NUM] ;
			int					m_Counters[CHANNEL_NUM] ;
			int					m_Polarity[CHANNEL_NUM] ;
			CHANNEL				m_LatchedChannel ;
			bool				m_IsToneLatched ;
			int					m_VolumeTable[16] ;
			int					m_CurrentBufferPos ;
			float				m_Cycles ;
			WORD				m_LFSR ;
			unsigned long int	m_ClockInfo ;
			float				m_BufferUpdateCount ;
			float				m_UpdateBufferLimit ;
};

#endif 
