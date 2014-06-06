#include "AppGateway.h"
#include "AppFrame.h"

#define TGW_BUFFER_SIZE 80

namespace Util
{
	PVoid fun_AppFrameGatewayRoutine(void* pArgs)
	{
		CAppGateway* pGateway = (CAppGateway*)pArgs;
		if (pGateway)
			pGateway->Run();
		
		return 0;
	}

	CAppGateway::CAppGateway(CAppFrame* pApp) : m_pApp(pApp), m_pThread(0), m_iPlatform(GATE_PF_NONE)
	{
	}

	
	CAppGateway::~CAppGateway()
	{
		DEF_RELEASE(m_pThread);
	}

	Bool CAppGateway::Init()
	{
		const CAppFrame::AppCfg& sCfg = *m_pApp->GetAppCfg();
		if (!BaseGateway::Init(sCfg.Gateway.Address,sCfg.Gateproxy.Address, sCfg.Gateway.Threads, sCfg.Gateway.Mode))
		{
			FmtPrint("AppGateway Init Error, Addr: %s, Threads: %d, Mode: %d", sCfg.Gateway.Address.c_str(), sCfg.Gateway.Threads, sCfg.Gateway.Mode);
			return false;
		}

		//加密模式
		if (sCfg.Gateway.Security)
			TurnGateSecurity();

		//性能监视
		if (sCfg.Gateway.Profiler.size())
			TurnOnProfiler(sCfg.Gateway.Profiler);

		//设置超时
		SetSessionTimeout(sCfg.Gateway.Timeout);

		//开启网关服务器线程
		if (!m_pThread)
		{
			m_pThread = new CThread(fun_AppFrameGatewayRoutine);
			m_pThread->Start(this);
		}

		return true;
	}

	Bool CAppGateway::Stop()
	{
		if (BaseGateway::Stop())
		{
			if (m_pThread)
			{
				m_pThread->Close();
				DEF_RELEASE(m_pThread);
			}
		}
		return true;
	}

	Bool CAppGateway::TrustSession(void* pSession, OctetsStream* pIBuffer)
	{
		if (!pSession || !pIBuffer)
			return false;
		
		//QQ平台网关校验信息过滤
		if (m_iPlatform == GATE_PF_QQ)
		{
			if (pIBuffer->AvailableSize() >= TGW_BUFFER_SIZE)
			{
				AString sTgw = (Char*)pIBuffer->AvailableData();

				#ifdef _DEBUG
					FmtPrint("TGW: %s", sTgw.c_str());
				#endif

					Bool bStart = CStringUtil::BeginWith<AString>(sTgw, "tgw_l7_forward");
					Bool bEnd   = CStringUtil::EndWith<AString>(sTgw, "\r\n\r\n");
				if (bStart && bEnd)
				{
					UInt32 iCrc  = COSOperator::CalcCrc((const UChar*)sTgw.c_str(),(UInt32)sTgw.size());
					UInt32* pCrc = (UInt32*)((Char*)pIBuffer->AvailableData() + (TGW_BUFFER_SIZE - sizeof(UInt32)));
					if (pCrc && iCrc == *pCrc)
					{
						pIBuffer->MoveNonius(TGW_BUFFER_SIZE);
						return true;
					}
				}
			}
			return false;
		}

		return true;
	}
}
