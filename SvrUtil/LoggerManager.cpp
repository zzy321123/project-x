#include "LoggerManager.h"
#include "StringUtil.h"
#include "OSOperator.h"
#include "Exception.h"

namespace Util
{
	//单例类实现
	CLoggerManager* CLoggerManager::m_pLoggerManager = 0;
	Bool CLoggerManager::InitInstance()				
	{											
		if(!m_pLoggerManager)								
		{											
			m_pLoggerManager = new CLoggerManager;					
			return true;								
		}												
		return false;								
	}												
	
	CLoggerManager* CLoggerManager::GetInstance()			
	{											
		A_Exception(m_pLoggerManager);						
		return m_pLoggerManager;							
	}												
			
	Bool CLoggerManager::ExitInstance()				
	{											
		if(m_pLoggerManager)								
		{											
			DEF_DELETE(m_pLoggerManager);						
			return true;								
		}												
		return false;								
	}

	CLoggerManager::CLoggerManager()
	{
		m_bConsole = true;
		m_pPrintLog = 0;
		m_sLogDir  = "Logs/";
		m_mLoggerMap.clear();		
	}

	CLoggerManager::~CLoggerManager()
	{		
		LoggerMap::iterator it = m_mLoggerMap.begin();
		while (it!=m_mLoggerMap.end())
		{
			CLogger* pLogger = it->second;
			DEF_RELEASE(pLogger);
			it++;
		}
		m_mLoggerMap.clear();
	}

	Bool CLoggerManager::SetupLogs(const AString& sDir)
	{
		m_sLogDir = sDir;
		CStringUtil::Replace<AString>(m_sLogDir, "\\","/");
		if (m_sLogDir.size() && m_sLogDir.at(m_sLogDir.size()-1) != '/')
			m_sLogDir += "/";

		//创建系统默认Log
		CreateLogger("ServerUtil.log",  "Util");
		CreateLogger("ServerError.log", "Error");
		CreateLogger("ServerPrint.log", "Print");

		//取出控制台日志
		m_pPrintLog = GetLogger("Print");

		return true;
	}

	CLogger* CLoggerManager::CreateLogger(const AString& sFile, const AString& sName, Bool bAppend)
	{
		CLogger* pLogger = GetLogger(sName);
		if(!pLogger)
		{
			AString sLog = m_sLogDir + sFile;
			COSOperator::MakeSureFileName(sLog);
			pLogger = new CLogger(sLog, bAppend);
			m_mLoggerMap[sName] = pLogger;
		}
		return pLogger;
	}
	
	CLogger* CLoggerManager::GetLogger(const AString& sName)
	{
		LoggerMap::iterator it = m_mLoggerMap.find(sName);
		if(it != m_mLoggerMap.end())
			return it->second;
		
		return 0;
	}

	void  CLoggerManager::EnableConsole(Bool bEnable)
	{
		m_bConsole = bEnable;
	}

	void  CLoggerManager::EnablePrintLog(Bool bEnable)
	{
		if (!bEnable)
			m_pPrintLog = 0;
		else
			m_pPrintLog = GetLogger("Print");
	}

	void  CLoggerManager::Print(const AString& sMsg)
	{
		if (sMsg.size())
		{
			if (m_bConsole)
			{
				printf("%s", sMsg.c_str());
			}
			
			if (m_pPrintLog)
			{
				m_pPrintLog->LogMsg(sMsg);
			}

#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
			OutputDebugStringA(sMsg.c_str());
#endif			
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	void  Print(const AString& sMsg, Bool bNewLine)
	{
		if (bNewLine)
			g_LoggerManager->Print(sMsg + "\r\n");	
		else
			g_LoggerManager->Print(sMsg);	
	}

	void FmtPrint(const Char* sFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE + 1] = {0};
		va_start(args, sFmt);
		_vsnprintf(sMsg, LOG_DEFAULT_SIZE, sFmt, args);
		va_end(args);

		Print(sMsg);
	}

	void FmtPrintLine(const Char* sFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE + 1] = {0};
		va_start(args, sFmt);
		_vsnprintf(sMsg, LOG_DEFAULT_SIZE, sFmt, args);
		va_end(args);

		Print(sMsg, false);
	}

	void Log(const AString& sMsg, CLogger::LogLevel eLevel, const AString& sName)
	{
		CLogger* pLogger = g_LoggerManager->GetLogger(sName);
		if (pLogger)
		{
			pLogger->LogMsg(sMsg + "\r\n", eLevel);
		}

#ifdef _DEBUG
		Print(sMsg, true);
#endif
	}

	void FmtLog(const Char* sFmt,  ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE + 1] = {0};
		va_start(args, sFmt);
		_vsnprintf(sMsg, LOG_DEFAULT_SIZE, sFmt, args);
		va_end(args);

		Log(sMsg);
	}

	void Error(const AString& sMsg, const AString& sName)
	{
		CLogger* pLogger = g_LoggerManager->GetLogger(sName);
		if (pLogger)
		{
			pLogger->LogMsg(sMsg + "\r\n", CLogger::LEVEL_ERROR);
		}

#ifdef _DEBUG
		Print(sMsg, true);
#endif
	}

	void FmtError(const Char* sFmt,  ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE + 1] = {0};
		va_start(args, sFmt);
		_vsnprintf(sMsg, LOG_DEFAULT_SIZE, sFmt, args);
		va_end(args);

		Error(sMsg);
	}
}
