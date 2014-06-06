#include "CLogServer.h"

namespace Util
{
	CLogServer::CLogServer()
	{		
		m_pLogDB	 = 0;
		m_pLogFile	 = 0;
		m_pLogCache  = 0;
		m_iCacheTime = 0;
		m_bConsole   = true;
		m_bRunning   = false;
		m_pFmtBuf    = (Utf8*)UtilMalloc(PAGE_SIZE);
		m_pRecvBuf	 = new OctetsStream(PAGE_SIZE);
	}

	CLogServer::~CLogServer()
	{
		m_sSocket.Close();

		UtilFree(m_pFmtBuf);
		DEF_RELEASE(m_pRecvBuf);		
		DEF_RELEASE(m_pLogCache);
		DEF_RELEASE(m_pLogFile);
	}

	Bool CLogServer::EnableConsole(Bool bEnable)
	{
		m_bConsole = bEnable;
		return m_bConsole;
	}

	Bool CLogServer::Init(const AString& sSvrAddr, const AString& sLogFile, Int32 iCacheSize, Int32 iCacheTime)
	{
		m_iCacheTime = iCacheTime;
		Assert(iCacheSize > 0);
		if(!m_sSocket.Create(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) 
		{
			Print("LogServer Init Socket Error.");
			return false;
		}

		m_sSocket.SetNoDelay(true);
		m_sSocket.SetBlocking(false);

		if(!m_sSocket.Bind(SocketAddr(sSvrAddr)))
		{
			Print("LogServer Bind Socket Error.");
			return false;
		}

		m_pLogFile = new CDiskFile;
		if (!m_pLogFile->Open(sLogFile, CFile::OPEN_APPEND))
		{
			Print("LogServer Open LogFile Error.");
			return false;
		}

		if (!m_pLogCache)
			m_pLogCache = new COctets(iCacheSize);
		
		return true;
	}

	Bool CLogServer::Init(const AString& sSvrAddr, const CDBConn& sConn, Int32 iCacheSize, Int32 iCacheTime)
	{
		m_iCacheTime = iCacheTime;
		Assert(iCacheSize > 0);
		if(!m_sSocket.Create(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) 
		{
			Print("LogServer Init Socket Error.");
			return false;
		}

		m_sSocket.SetNoDelay(true);
		m_sSocket.SetBlocking(false);

		if(!m_sSocket.Bind(SocketAddr(sSvrAddr)))
		{
			Print("LogServer Bind Socket Error.");
			return false;
		}

		m_pLogDB = g_DBManager->OpenDatabase("LogDB", sConn);
		if (!m_pLogDB)
		{
			Print("LogServer Open LogDB Error.");
			return false;
		}

		if (!m_pLogCache)
			m_pLogCache = new COctets(iCacheSize);

		return true;
	}

	Bool CLogServer::Run()
	{
		if (!m_pLogFile && !m_pLogDB)
			return false;

		if (m_bRunning || !m_pRecvBuf || !m_pLogCache)
			return false;

		m_bRunning = true;
		UInt32 iFlushTime = COSOperator::GetTickCount();
			
		while(m_bRunning)
		{
			UInt32 iCurTime = COSOperator::GetTickCount();
			if (!m_sSocket.UpdateEvent(HEVENT_READ))
			{
				if (iCurTime - iFlushTime >= (UInt32)m_iCacheTime)
				{
					FlushLogs();
					iFlushTime = iCurTime;
				}
				else
				{
					Sleep(DEFAULT_SLEEP);
				}
				continue;
			}

			m_pRecvBuf->Clear();
			Size_t iRecv = (Size_t)m_pRecvBuf->Capacity();

			SocketAddr sAddr;
			if(!m_sSocket.ReceiveFrom(m_pRecvBuf->Begin(), iRecv, sAddr))
				continue;

			m_pRecvBuf->Resize((UInt32)iRecv);
			Protocol* pProto = 0;
			try
			{
				pProto = g_ProtocolManager->Decode(*m_pRecvBuf);
				if (!pProto) continue;
			}
			catch (CException& rhsExcep)
			{
#ifdef _DEBUG
				FmtPrint("Exception: %s", rhsExcep.GetMsg().c_str());
#endif
				rhsExcep.Throw();
				continue;
			}

			ProtoType eType = pProto->GetType();
			if(eType == SysProtocol::SYS_LOG_MSG)
			{
				SysProtocol::Sys_LogMsg* pCmd = (SysProtocol::Sys_LogMsg*)pProto;
				CacheLogs(pCmd->m_iSvrId, pCmd->m_iUsrId, pCmd->m_sUsrName, pCmd->m_iType, pCmd->m_sKey, pCmd->m_sMsg);
			}

			g_ProtocolManager->ReleaseProto(pProto);
		}

		FlushLogs();
		return true;
	}

	Bool CLogServer::CacheLogs(Int32 iSvrId, UInt32 iUsrId, const UString& sUsrName, Int32 iType, const UString& sKey, const UString& sMsg)
	{
		memset(m_pFmtBuf, 0, PAGE_SIZE);
		if (!sKey.size() || !sMsg.size())
			return false;

		if (m_pLogFile)
		{
			//格式化日志信息
			sprintf((Char*)m_pFmtBuf, "SvrId: %d, UsrId: %u, UsrName: %s, LogType: %d, LogTime: %s, LogKey: %s, LogMsg: %s",
				iSvrId, iUsrId, sUsrName.c_str(), iType, COSOperator::GetTimeString().c_str(), sKey.c_str(), sMsg.c_str());

			UInt32 iSize = (UInt32)strlen((Char*)m_pFmtBuf);
			if (iSize + m_pLogCache->Size() >= m_pLogCache->Capacity() - 2)
			{
				//日志落地
				FlushLogs();
			}

			//屏幕输出
			if (m_bConsole)
				Print(CStringUtil::ToString(m_pFmtBuf));

			//写入缓存
			m_pLogCache->Append(m_pFmtBuf, iSize);
			return true;
		}
		else if (m_pLogDB)
		{
			//格式化日志信息
			if(m_pLogCache->Size())
			{
				sprintf((Char*)m_pFmtBuf, ",(%d, %u, '%s', %d, '%s', '%s', '%s')",
					iSvrId, iUsrId, sUsrName.c_str(), iType, COSOperator::GetTimeString().c_str(), sKey.c_str(), sMsg.c_str());				
			}
			else
			{
				sprintf((Char*)m_pFmtBuf, "INSERT INTO logs(SvrId, UsrId, UsrName, LogType, LogTime, LogKey, LogMsg) VALUES(%d, %u, '%s', %d, '%s', '%s', '%s')",
					iSvrId, iUsrId, sUsrName.c_str(), iType, COSOperator::GetTimeString().c_str(), sKey.c_str(), sMsg.c_str());
			}

			UInt32 iSize = (UInt32)strlen((Char*)m_pFmtBuf);
			if (iSize + m_pLogCache->Size() >= m_pLogCache->Capacity() - 2)
			{
				//日志落地
				FlushLogs();

				//重新Format日志格式
				sprintf((Char*)m_pFmtBuf, "INSERT INTO logs(SvrId, UsrId, UsrName, LogType, LogTime, LogKey, LogMsg) VALUES(%d, %u, '%s', %d, '%s', '%s', '%s')",
					iSvrId, iUsrId, sUsrName.c_str(), iType, COSOperator::GetTimeString().c_str(), sKey.c_str(), sMsg.c_str());

				iSize = (UInt32)strlen((Char*)m_pFmtBuf);
			}

			//屏幕输出
			if (m_bConsole)
				Print(CStringUtil::ToString(m_pFmtBuf));

			//写入缓存
			m_pLogCache->Append(m_pFmtBuf, iSize);
			return true;
		}
		return false;
	}

	Bool CLogServer::FlushLogs()
	{
		if (m_pLogCache->Size())
		{
			if (m_pLogFile)
			{
				m_pLogFile->Write(m_pLogCache->Begin(), m_pLogCache->Size(), true);
			}
			else if (m_pLogDB)
			{
				Utf8 cEnd[] = {';', 0};
				m_pLogCache->Append(cEnd, 2);

				if(!m_pLogDB->Execute((Utf8*)m_pLogCache->Begin()))
				{
					FmtPrint("FlushLogs Error, ErrId: %d, ErrMsg: %s", m_pLogDB->GetErrCode(), (Char*)m_pLogDB->GetErrMsg().c_str());
				}
			}
		}

		m_pLogCache->Clear();
		return true;
	}

	Bool CLogServer::Stop()
	{
		if (m_bRunning)
		{
			m_bRunning = false;
			return true;
		}
		return false;
	}
}
