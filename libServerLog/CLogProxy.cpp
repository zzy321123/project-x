#include "CLogProxy.h"

namespace Util
{
	CLogProxy::CLogProxy() : m_iSvrId(0), m_bConsole(false), m_bShowThread(false)
	{
		m_pLock = new CMutex;

#ifdef _DEBUG
		m_bConsole = true;
#endif
	}

	CLogProxy::~CLogProxy()
	{
		m_sSocket.Close();
		DEF_RELEASE(m_pLock);
	}

	Bool CLogProxy::Init(const AString& sAddr, UInt32 iSvrId)
	{
		Assert(sAddr.size());
		m_sAddr  = SocketAddr(sAddr);
		m_iSvrId = iSvrId;

		if(!m_sSocket.Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) 
		{
			Print("LogProxy Create Socket Error.");
			return false;
		}

		m_sSocket.SetNoDelay(true);
		m_sSocket.SetBlocking(false);

		CSocket::MaximizeSndBuf(m_sSocket.Handle());

		return true;
	}

	void CLogProxy::EnableConsole(Bool bEnable)
	{
		m_bConsole = bEnable;
	}

	void CLogProxy::SetShowThread(Bool bShow)
	{
		m_bShowThread = bShow;
	}

	Bool  CLogProxy::FmtMsg(const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};
		va_start(args,(const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(0, "", LT_MSG, pKey, sMsg);
	}

	Bool CLogProxy::FmtWarn(const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};
		va_start(args, (const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(0, "", LT_WARN, pKey, sMsg);
	}

	Bool CLogProxy::FmtError(const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};		
		va_start(args, (const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(0, "", LT_ERROR, pKey, sMsg);
	}

	Bool CLogProxy::FmtMsg(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};
		va_start(args,(const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(iUsrId, pUsrName, LT_MSG, pKey, sMsg);
	}

	Bool CLogProxy::FmtWarn(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};
		va_start(args, (const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(iUsrId, pUsrName, LT_WARN, pKey, sMsg);
	}

	Bool CLogProxy::FmtError(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		va_list args;
		Char sMsg[LOG_DEFAULT_SIZE] = {0};		
		va_start(args, (const Char*)pFmt);
		_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE-1, (Char*)pFmt, args);
		va_end(args);

		return SendLog(iUsrId, pUsrName, LT_ERROR, pKey, sMsg);
	}

	Bool CLogProxy::SendLog(UInt8 iType, const Char* pKey, const Char* pMsg)
	{
		return SendLog(0, "", iType, pKey, pMsg);
	}

	Bool CLogProxy::SendLog(UInt32 iUsrId, const Char* pUsrName, UInt8 iType, const Char* pKey, const Char* pMsg)
	{
		Assert(pKey && pMsg);
		SysProtocol::Sys_LogMsg sCmd;
		sCmd.m_iSvrId = m_iSvrId;
		sCmd.m_iUsrId = iUsrId;
		sCmd.m_iType  = iType;

		//避免空指针赋值
		if (pUsrName)	sCmd.m_sUsrName	= (Utf8*)pUsrName;
		if (pKey)		sCmd.m_sKey		= (Utf8*)pKey;
		if (pMsg)		sCmd.m_sMsg		= (Utf8*)pMsg;

		//编码为数据流
		OctetsStream xOS;
		sCmd.Encode(xOS);

		//发送日志数据
		Size_t iSize = (Size_t)xOS.Size();
		if(iSize) 
		{
			AutoMutex(lock,m_pLock);
			m_sSocket.SendTo(xOS.Begin(), iSize, m_sAddr);
		}

		//控制台打印
		if (m_bConsole)
		{
			if (m_bShowThread)
			{
				FmtPrint("Thread = %u, UsrId = %u, UserName = %s, Type = %u, Key = %s, Msg = %s", 
					COSOperator::GetThreadId(), sCmd.m_iUsrId, sCmd.m_sUsrName.c_str(), sCmd.m_iType, sCmd.m_sKey.c_str(), sCmd.m_sMsg.c_str());
			}
			else
			{
				FmtPrint("UsrId = %u, UserName = %s, Type = %u, Key = %s, Msg = %s", 
					sCmd.m_iUsrId, sCmd.m_sUsrName.c_str(), sCmd.m_iType, sCmd.m_sKey.c_str(), sCmd.m_sMsg.c_str());
			}
		}

		return true;
	}
}
