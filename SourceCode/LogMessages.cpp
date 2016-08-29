#include "LogMessages.h"
#include "Config.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////

LogMessage* LogMessage::m_Instance = NULL;

LogMessage* LogMessage::CreateInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new LogMessage();
	}

	return m_Instance;
}

////////////////////////////////////////////////////////////////////////////////////////

LogMessage* LogMessage::GetSingleton()
{
	assert(NULL != m_Instance);
	return m_Instance;
}

////////////////////////////////////////////////////////////////////////////////////////

void LogMessage::DoLogMessage(const char* message, bool logToConsole)
{
	if (NULL != m_LogFile)
	{
		long long size = ftell(m_LogFile);
		if (size >= (30 * 1024 * 1024))
		{
			fclose(m_LogFile);
			Open();
		}

		fputs(message, (FILE*)m_LogFile);
		fputs("\r\n", (FILE*)m_LogFile);
	}

	if (!logToConsole)
	{
		return;
	}

#ifdef WIN32
	OutputDebugStr(message);
	OutputDebugStr("\n");
#else
	printf(message);
	printf("\n");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////

LogMessage::LogMessage(void) : m_LogFile(NULL), m_FileNum(1)
{
	Open();
}

////////////////////////////////////////////////////////////////////////////////////////

LogMessage::~LogMessage(void)
{
	if (NULL != m_LogFile)
	{
		fclose((FILE*)m_LogFile);
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void LogMessage::Open()
{
	char buffer[100];
	sprintf(buffer, "emulator%d.log", m_FileNum) ;
	m_FileNum++ ;
	m_LogFile = (_iobuf*)fopen(buffer, "w") ;
}

////////////////////////////////////////////////////////////////////////////////////////