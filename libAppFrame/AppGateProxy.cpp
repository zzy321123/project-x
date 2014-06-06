#include "AppGateProxy.h"
#include "AppFrame.h"

namespace Util
{
	CAppGateProxy::CAppGateProxy(CAppFrame* pApp) : m_pApp(pApp), m_bConnect(false)
	{
	}

	CAppGateProxy::~CAppGateProxy()
	{
	}

	Bool CAppGateProxy::Init()
	{
		const CAppFrame::AppCfg& sCfg = *m_pApp->GetAppCfg();
		if (!BaseGateProxy::Init(sCfg.Gateproxy.Address, sCfg.AppId, sCfg.Gateproxy.ConnLimit))
		{
			FmtPrint("AppGateProxy Init Error, Addr: %s.", sCfg.Gateproxy.Address.c_str());
			return false;
		}

		//向网关服务器协议注册
		vector<ProtoType> vType;
		vector<UInt8>	  vDrive;				
		g_ProtocolManager->GetRegProtoIds(vType, &vDrive);

		map<ProtoType, UInt8> mTypeDrive;
		Assert(vType.size() == vDrive.size());
		if (vType.size() == vDrive.size())
		{
			for (Size_t i=0;i<vType.size();i++)
				mTypeDrive[ vType[i] ] = vDrive[i];
		}
		RegisterProtoType(mTypeDrive);

		//注册网关代理器线程
		vector<UInt32> vThreads;
		vThreads.push_back(COSOperator::GetThreadId());
		for (Int32 i=0;i<m_pApp->m_pThreadPool->GetThreadNum();i++)
		{
			vThreads.push_back((UInt32)m_pApp->m_pThreadPool->GetThreadId(i));
			FmtPrint("AppThread: %d", m_pApp->m_pThreadPool->GetThreadId(i));
		}
		if (!RegisterThreads(vThreads))
		{
			FmtPrint("GateProxy Register Threads Error.");
			return false;
		}

		return true;
	}

	Bool CAppGateProxy::OnProxyConnected()
	{
		m_bConnect = true;

		if (m_pApp)
			m_pApp->OnGateProxyConnected();

		return BaseGateProxy::OnProxyConnected();
	}

	Bool CAppGateProxy::OnProxyDisconnect()
	{
		m_bConnect = false;

		if (m_pApp)
			m_pApp->OnGateProxyDisconnect();

		return BaseGateProxy::OnProxyDisconnect();
	}

	Bool CAppGateProxy::IsConnect() const
	{
		return m_bConnect;
	}

	Bool CAppGateProxy::OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto)
	{
		Assert(pProto);
		if (!m_pApp || !pProto) 
			return false;

		//指定会话ID的是由用户发送过来的
		if (sHeader.Sid)
		{
			return m_pApp->OnSessionProtocol(sHeader.Sid, pProto);
		}

		//系统间的协议处理
		ProtoType iType = pProto->GetType();
		if (iType == SysProtocol::SYS_SESSION_START)
		{
			SysProtocol::Sys_SessionStart* pCmd = (SysProtocol::Sys_SessionStart*)pProto;
			if (pCmd->m_iSid)
				m_pApp->OnSessionStart(pCmd->m_iSid, pCmd->m_sAddr);
		}
		else if (iType == SysProtocol::SYS_SESSION_BREAK)
		{
			SysProtocol::Sys_SessionBreak* pCmd = (SysProtocol::Sys_SessionBreak*)pProto;
			if (pCmd->m_iSid)
				m_pApp->OnSessionClose(pCmd->m_iSid, pCmd->m_sAddr);
		}
		else
		{
			Assert(false && "Exception Protocol.");
		}

		//最终释放协议
		g_ProtocolManager->ReleaseProto(pProto);

		return true;
	}
}
