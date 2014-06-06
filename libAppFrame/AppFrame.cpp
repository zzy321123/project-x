#include "AppFrame.h"

namespace Util
{
	CAppFrame::CAppFrame(const XID& sXid) : CApp(sXid)
	{
		m_pLogProxy	  = 0;	
		m_pGateProxy  = 0;
		m_iAppId	  = 0;
		m_pGateway	  = 0;
		m_iElapseTime = 0;
		m_bBreakLoop  = false;
		m_iSysTime	  = (UInt32)COSOperator::GetSysTime();
		m_iTickTime	  = COSOperator::GetTickCount();
	}

	CAppFrame::~CAppFrame()
	{
		m_mSidAddr.Clear();

		if (m_mDbHdl.size())
		{
			DBHdlMap::iterator it = m_mDbHdl.begin();
			for (;it!=m_mDbHdl.end();it++)
			{
				g_DBManager->CloseDatabase(it->second->GetDBName());
			}
			m_mDbHdl.clear();
		}		

		DEF_RELEASE(m_pLogProxy);
		DEF_RELEASE(m_pGateProxy);
		DEF_RELEASE(m_pGateway);
	}	

	CMysql* CAppFrame::GetSafeDbHdl()
	{
		UInt32 dwThreadId = COSOperator::GetThreadId();
		DBHdlMap::iterator it = m_mDbHdl.find(dwThreadId);
		if (it != m_mDbHdl.end())
			return it->second;

		return 0;
	}

	Bool CAppFrame::BreakLoop()
	{
		m_bBreakLoop = true;
		return true;
	}

	const CAppFrame::AppCfg* CAppFrame::GetAppCfg() const
	{
		return &m_sAppCfg;
	}

	UInt32 CAppFrame::GetAppId() const
	{
		return m_iAppId;
	}

	UInt32 CAppFrame::GetSysTime() const
	{
		return m_iSysTime;
	}

	UInt32 CAppFrame::GetTickTime() const
	{
		return m_iTickTime;
	}
	
	UInt32 CAppFrame::GetElapseTime() const
	{
		return m_iElapseTime;
	}

	AString	CAppFrame::GetSidAddr(SID iSid)
	{
		AString sAddr;
		if (m_mSidAddr.Find(iSid, &sAddr))
			return sAddr;

		return "";
	}	

	Bool CAppFrame::Init(const AppCfg& sCfg)
	{
		m_sAppCfg = sCfg;
		Assert(m_sAppCfg.Threads && m_sAppCfg.AppId);
		if (CApp::Init(m_sAppCfg.Threads))
		{
			//日志对象初始化
			if (m_sAppCfg.LogAddr.size())
			{
				if (!m_pLogProxy)
					m_pLogProxy = new CLogProxy;

				if (!m_pLogProxy->Init(m_sAppCfg.LogAddr, m_sAppCfg.AppId))
				{
					FmtPrint("LogProxy Init Error, Addr: %s.", m_sAppCfg.LogAddr.c_str());
					return false;
				}
				m_pLogProxy->EnableConsole(m_sAppCfg.Console);
			}
			g_LoggerManager->EnableConsole(m_sAppCfg.Console);

			//内嵌网关模式
			if (sCfg.Gateway.Address.size())
			{
				m_pGateway = new CAppGateway(this);
				if (!m_pGateway->Init())
					return false;
			}

			//网关代理器
			m_pGateProxy = new CAppGateProxy(this);
			if (!m_pGateProxy->Init())
				return false;

			//数据库对象初始化
			if (sCfg.DBConn.IsValid())
			{
				vector<UInt32> vThreads;
				vThreads.push_back(COSOperator::GetThreadId());
				for (Int32 i=0; i < m_pThreadPool->GetThreadNum(); i++)
				{
					vThreads.push_back((UInt32)m_pThreadPool->GetThreadId(i));
					FmtPrint("AppThread: %d", m_pThreadPool->GetThreadId(i));
				}

				for (Size_t i=0; i < vThreads.size(); i++)
				{
					Char sDBName[DEFAULT_SIZE] = {0};
					sprintf(sDBName,"AppDB_%u", vThreads[i]);
					CMysql* pDBHdl = (CMysql*)g_DBManager->OpenDatabase(sDBName, m_sDbConn);
					if(!pDBHdl) 
						return false;

					m_mDbHdl[ vThreads[i] ] = pDBHdl; 	
				}
			}

			return true;
		}
		return false;
	}
	
	Bool CAppFrame::Run()
	{
		if (CApp::Run())
		{
			UInt32 iTickTime = 0;
			while (m_bRunning && !m_bBreakLoop)
			{
				m_iSysTime	  = (UInt32)COSOperator::GetSysTime();
				iTickTime	  = COSOperator::GetTickCount();
				m_iElapseTime = iTickTime - m_iTickTime;
				m_iTickTime   = iTickTime;

				//Util体系帧更新
				CSvrUtil::Tick(m_iElapseTime);
			
				//网关代理器更新
				if (m_pGateProxy)
					m_pGateProxy->Update();

				//逻辑帧更新
				OnTick(m_iElapseTime);
			}

			//等待线程结束
			Stop();

			return true;
		}
		return false;
	}
	
	Bool CAppFrame::SendProtocol(SID iSid, Protocol* pProto)
	{
		Assert(iSid && pProto);

		if (!iSid || !pProto || !m_pGateProxy || !m_pGateProxy->IsConnect())
			return false;

		return m_pGateProxy->SendProtocol(iSid, pProto);
	}

	Bool CAppFrame::CloseSession(SID iSid)
	{
		if (iSid)
		{
			m_mSidAddr.Erase(iSid);

			if (m_pGateProxy)
				return m_pGateProxy->CloseSession(iSid);
		}
		return false;
	}

	Bool CAppFrame::BroadcastMsg(ObjMan* pObjMan, CMsg* pMsg)
	{
		if (pObjMan && pMsg)
		{
			XIDVector vXID;
			pObjMan->CollectObjKey(vXID);

			if (vXID.size())
				PostMsg(vXID, pMsg);

			return true;
		}		
		return false;
	}

	Bool CAppFrame::BroadcastProto(Protocol* pProto, const vector<SID>& vSid)
	{
		if (!pProto || !m_pGateProxy || !m_pGateProxy->IsConnect())
			return false;

		if (!vSid.size())
		{
			vector<SID> vAllSid;
			m_mSidAddr.CollectKey(vAllSid);
			return m_pGateProxy->BroadcastProto(vAllSid, pProto);
		}

		return m_pGateProxy->BroadcastProto(vSid, pProto);
	}

	Bool CAppFrame::FormatMsg(const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtMsg(0, "", pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::FormatWarn(const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtWarn(0, "", pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::FormatError(const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtError(0, "", pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::FormatMsg(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtMsg(iUsrId, pUsrName, pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::FormatWarn(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtWarn(iUsrId, pUsrName, pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::FormatError(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...)
	{
		if (m_pLogProxy)
		{
			va_list vArgs;
			Char sMsg[LOG_DEFAULT_SIZE+1] = {0};
			va_start(vArgs, (const Char*)pFmt);
			_vsnprintf((Char*)sMsg, LOG_DEFAULT_SIZE, (Char*)pFmt, vArgs);
			va_end(vArgs);

			return m_pLogProxy->FmtError(iUsrId, pUsrName, pKey, sMsg);
		}

		return false;
	}

	Bool CAppFrame::OnGateProxyConnected()
	{
		m_mSidAddr.Clear();		
		return true;
	}

	Bool CAppFrame::OnSessionStart(SID iSid, const AString& sAddr)
	{
		m_mSidAddr.Set(iSid, sAddr);
		FormatMsg("SessionStart", "Sid: %u, IpAddr: %s", iSid, sAddr.c_str());
		return true;
	}

	Bool CAppFrame::OnSessionProtocol(SID iSid, Protocol* pProto)
	{
		if (iSid && pProto)
		{
			PostProtocol(iSid, pProto);
			return true;
		}
		return false;
	}

	Bool CAppFrame::OnSessionClose(SID iSid, const AString& sAddr)
	{
		if (m_mSidAddr.Erase(iSid))
		{
			FormatMsg("SessionClose", "Sid: %u, IpAddr: %s", iSid, sAddr.c_str());
			return true;
		}
		return false;
	}

	Bool CAppFrame::OnGateProxyDisconnect()
	{
		m_mSidAddr.Clear();
		return true;
	}

	Bool CAppFrame::OnAcceptConnect(const CSocket& sSocket, const SocketAddr& sAddr)
	{
		return true;
	}

	CAppObj* CAppFrame::AppCreateObj(const XID& sXid)
	{
		return CApp::AppCreateObj(sXid);
	}

	Bool CAppFrame::OnTick(UInt32 iPeriod)
	{
		return true;
	}

	Bool CAppFrame::Stop()
	{
		//网关服务器线程关闭
		if (m_pGateway)
			m_pGateway->Stop();

		return CApp::Stop();
	}	
}
