#include "CProfiler.h"

namespace Util
{
	extern UInt32 get_processor_number();
	extern UInt64 get_phys_mem();
	extern UInt32 get_cpu_usage();
	extern UInt64 get_memory_usage();

	PVoid fun_ProfilerRoutine(void* pArgs)
	{
		CProfiler* pProfiler = static_cast<CProfiler*>(pArgs); 
		if (pProfiler)
			pProfiler->RunLoop();

		return 0;
	}

	CProfiler::CProfiler(const AString& sAddr)
	{
		m_iCpuCount = 0;
		m_iTotalMem	= 0;
		m_iCpuUsage = 0;
		m_iMemUsage = 0;
		m_pThread	= 0;
		m_iConnect	= 0;
		m_iRecvProto= 0;
		m_iRecvSize	= 0;
		m_iSendProto= 0;
		m_iSendSize	= 0;
		m_bRunning  = false;
		m_sAddr		= sAddr;
		m_pRecvBuf	= new OctetsStream(PAGE_SIZE);
	}

	CProfiler::~CProfiler()
	{
		m_sSocket.Close();

		DEF_RELEASE(m_pThread);
		DEF_RELEASE(m_pRecvBuf);
	}

	Bool CProfiler::RegRecvProto(ProtoType iType, ProtoSize iSize)
	{
		if (iType)
			CAtomic::Inc<UInt64>(&m_iRecvProto);

		if (iSize)
			CAtomic::Add<UInt64>(&m_iRecvSize, iSize);

		return true;
	}

	Bool CProfiler::RegSendProto(ProtoType iType, ProtoSize iSize)
	{
		if (iType)
			CAtomic::Inc<UInt64>(&m_iSendProto);

		if (iSize)
			CAtomic::Add<UInt64>(&m_iSendSize, iSize);

		return true;
	}

	Bool CProfiler::RegConnect(Bool bConn)
	{
		if (bConn)
			CAtomic::Inc<UInt32>(&m_iConnect);
		else
			CAtomic::Dec<UInt32>(&m_iConnect);

		return true;
	}

	UInt32 CProfiler::GetProcessorNumber()
	{
		if (!m_iCpuCount)
			m_iCpuCount = get_processor_number();

		return m_iCpuCount;
	}

	UInt64 CProfiler::GetTotalMem()
	{
		if (!m_iTotalMem)
			m_iTotalMem = get_phys_mem();

		return m_iTotalMem;
	}

	UInt32 CProfiler::GetCpuUsage()
	{
		m_iCpuUsage = get_cpu_usage();
		return m_iCpuUsage;
	}

	UInt64 CProfiler::GetMemUsage()
	{
		m_iMemUsage = get_memory_usage();
		return m_iMemUsage;
	}

	Bool CProfiler::Start(const AString& sAddr)
	{
		if (!m_bRunning)
		{
			if (sAddr.size())
				m_sAddr = sAddr;

			GetProcessorNumber();
			GetTotalMem();
			GetCpuUsage();
			GetMemUsage();

			if(!m_sSocket.Create(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) 
			{
				Print("Profiler Init Socket Error.");
				return false;
			}

			m_sSocket.SetNoDelay(true);
			m_sSocket.SetBlocking(false);

			if(!m_sSocket.Bind(SocketAddr(m_sAddr)))
			{
				Print("Profiler Bind Socket Error.");
				return false;
			}

			m_bRunning = true;
			m_pThread  = new CThread(fun_ProfilerRoutine);
			m_pThread->Start(this);

			return true;
		}		
		return false;
	}

	Bool CProfiler::Stop()
	{
		m_bRunning = false;

		if (m_pThread)
			m_pThread->Close();

		return true;
	}

	Bool CProfiler::RunLoop()
	{
		UInt32 iCalcTime = COSOperator::GetTickCount();

		while(m_bRunning)
		{
			UInt32 iCurTime = COSOperator::GetTickCount();

			//定期计算CPU和内存使用情况
			if (iCurTime -  iCalcTime >= 1000)
			{
				GetCpuUsage();
				GetMemUsage();
				iCalcTime = iCurTime;
			}

			if (!m_sSocket.UpdateEvent(HEVENT_READ, DEFAULT_SLEEP))
				continue;

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
				FmtPrint("Exception: %s", rhsExcep.GetMsg().c_str());
				continue;
			}

			ProtoType eType = pProto->GetType();
			if(eType == SysProtocol::SYS_PROF_REQ)
			{
				SysProtocol::Sys_ProfInfo sInfo;
				sInfo.m_iTimeStamp	 = COSOperator::GetTickCount();
				sInfo.m_iCpuCount	 = m_iCpuCount;
				sInfo.m_iTotalMem	 = m_iTotalMem;
				sInfo.m_iCpuUsage	 = m_iCpuUsage;
				sInfo.m_iMemUsage	 = m_iMemUsage;
				sInfo.m_iConnect	 = m_iConnect;
				sInfo.m_iRecvProto	 = m_iRecvProto;
				sInfo.m_iRecvSize	 = m_iRecvSize;
				sInfo.m_iSendProto	 = m_iSendProto;
				sInfo.m_iSendSize	 = m_iSendSize;

				m_pRecvBuf->Clear();
				sInfo.Encode(*m_pRecvBuf);
				Size_t iSize = (Size_t)m_pRecvBuf->Size();
				m_sSocket.SendTo(m_pRecvBuf->Begin(), iSize, sAddr);
			}

			g_ProtocolManager->ReleaseProto(pProto);
		}
		return true;
	}
}
