#include "BaseGateProxy.h"
#include "BaseGateDef.h"
#include "zmq.h"

namespace Util
{
	BaseGateProxy::BaseGateProxy()
	{
		m_pProxyZmq   = 0;
		m_pOctets     = 0;		
		m_pCollectZmq = 0;
		m_iConnLimit  = 0;
		m_iProxyState = UTIL_ERROR;
	}

	BaseGateProxy::~BaseGateProxy()
	{
		ThreadProxyMap::iterator it = m_mThreadProxy.begin();
		for (; it != m_mThreadProxy.end(); it++)
			g_ZmqManager->CloseZmq(it->second);

		m_mThreadProxy.clear();
		DEF_RELEASE(m_pOctets);

		g_ZmqManager->CloseZmq(m_pCollectZmq);
		g_ZmqManager->CloseZmq(m_pProxyZmq);			
	}

	Bool BaseGateProxy::Init(const AString& sAddr, UInt32 iSvrId, UInt32 iConnLimit)
	{
		if (!m_pOctets)
			m_pOctets = new OctetsStream(PAGE_SIZE*2);

		m_iSvrId     = iSvrId;
		m_sGateAddr  = sAddr;
		m_iConnLimit = iConnLimit;
		if (m_sGateAddr.find("tcp://") == AString::npos)
			m_sGateAddr = "tcp://" + sAddr;	

		if (!m_pProxyZmq)
		{
			m_pProxyZmq = g_ZmqManager->CreateZmq(CZmq::HZMQ_DEALER);

			UInt64 iZmqId = CZmq::GenValidZid(m_iSvrId);
			if (!m_pProxyZmq->SetIdentity(&iZmqId, (Int32)sizeof(iZmqId)))
				return false;

			m_pProxyZmq->StartMonitor(CZmq::HZMQ_EVENT_CONNECTED | CZmq::HZMQ_EVENT_DISCONNECTED);
			if (!m_pProxyZmq->Connect(m_sGateAddr))
				return false;
		}

		if (!m_pCollectZmq)
		{
			m_pCollectZmq = g_ZmqManager->CreateZmq(CZmq::HZMQ_PULL);

			Char szCollectAddr[DEFAULT_SIZE] = {0};
			Long lCollectId = (Long)m_pCollectZmq->GetHandle();
			sprintf(szCollectAddr, "inproc://server-gateproxy-%ld", lCollectId);

			if (!m_pCollectZmq->Bind(szCollectAddr))
			{
				FmtPrint("CollectZmq Bind Error, Addr: %s", szCollectAddr);
				return false;
			}			
		}

		return true;
	}

	Bool BaseGateProxy::RecvProtocol(GateMsgHeader& sHeader, CProtocol*& pProto, Int32 iTimeout)
	{
		if (m_pProxyZmq && m_pOctets)
		{
			if (m_pProxyZmq->PollEvent(HEVENT_READ, iTimeout))
			{
				//提取消息头
				m_pOctets->Clear();
				Size_t iSize = (Size_t)m_pOctets->Capacity();
				if (!m_pProxyZmq->Recv(m_pOctets->Begin(), iSize))
					return false;

				sHeader = *((GateMsgHeader*)m_pOctets->Begin());
				Bool bRecvMore = m_pProxyZmq->IsWaitRecv();			
				Assert(iSize == sizeof(sHeader) && bRecvMore);
				if (iSize != sizeof(sHeader) || !bRecvMore)
					return false;

				//提取协议内容
				m_pOctets->Clear();
				iSize = (Size_t)m_pOctets->Capacity();
				if (!m_pProxyZmq->Recv(m_pOctets->Begin(), iSize))
					return false;				

				//协议解析
				m_pOctets->Resize(iSize);
				try
				{
					pProto = g_ProtocolManager->Decode(*m_pOctets);
				}
				catch (CException& rhsExcep)
				{
					//协议解析异常退出
					FmtError("Session Decode Protocol Error, Msg: %s", rhsExcep.GetMsg().c_str());
					//释放协议
					g_ProtocolManager->ReleaseProto(pProto);
					return false;
				}

				Assert(!m_pProxyZmq->IsWaitRecv());

				return pProto != 0;
			}			
		}
		return false;
	}	

	Bool  BaseGateProxy::CloseSession(SID iSid)
	{
		if (iSid)
		{
			GateNotify sNotify(GateNotify::NOTIFY_SESSION_CLOSE);
			sNotify.eClose.SvrId = m_iSvrId;
			sNotify.eClose.Sid   = iSid;			
			return SendNotify(sNotify);
		}
		return false;
	}

	Bool BaseGateProxy::RegisterProtoType(const ProtoTypeDriveMap& mTypeDrive)
	{
		if (&mTypeDrive != &m_mProtoTypeDrive)
			m_mProtoTypeDrive = mTypeDrive;

		if (m_iProxyState == UTIL_OK && m_mProtoTypeDrive.size())
		{
			OctetsStream xOS;
			xOS << m_mProtoTypeDrive;

			GateNotify sNotify(GateNotify::NOTIFY_PROTO_REGISTER);			
			return SendNotify(sNotify, &xOS);
		}
		return true;
	}	

	Bool BaseGateProxy::RegisterThreads(const vector<UInt32>& vThreads)
	{
		Char szCollectAddr[DEFAULT_SIZE] = {0};
		Long lCollectId = (Long)m_pCollectZmq->GetHandle();
		sprintf(szCollectAddr, "inproc://server-gateproxy-%ld", lCollectId);

		for (Size_t i=0;i<vThreads.size();i++)
		{
			UInt32 iThreadId = vThreads[i];

			//支持多次调用
			if (m_mThreadProxy.find(iThreadId) != m_mThreadProxy.end())
			{
				FmtError("GateProxy Thread Register Duplicate, ThreadId: %u", iThreadId);
				continue;
			}

			CZmq* pZmq = g_ZmqManager->CreateZmq(CZmq::HZMQ_PUSH);

			UInt64 iZmqId = CZmq::GenValidZid(iThreadId);
			if (!pZmq->SetIdentity(&iZmqId, (Int32)sizeof(iZmqId)))
			{
				g_ZmqManager->CloseZmq(pZmq);
				return false;
			}

			if (!pZmq->Connect(szCollectAddr))
			{
				g_ZmqManager->CloseZmq(pZmq);
				return false;
			}

			m_mThreadProxy[iThreadId] = pZmq;
		}

		return true;
	}

	CZmq* BaseGateProxy::GetThreadProxy()
	{
		UInt32 iThreadId = COSOperator::GetThreadId();
		ThreadProxyMap::iterator it = m_mThreadProxy.find(iThreadId);
		if (it != m_mThreadProxy.end())
			return it->second;

		return 0;
	}

	Bool BaseGateProxy::OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto)
	{
		return false;
	}

	Bool BaseGateProxy::SendProtocol(SID iSid, CProtocol* pProto)
	{
		SvrMsgHeader sHeader;
		sHeader.Sid = iSid;
		return SendProtocol(sHeader, pProto);
	}

	Bool BaseGateProxy::SendProtocol(const SvrMsgHeader& sHeader, CProtocol* pProto)
	{
		if (m_iProxyState == UTIL_OK)
		{
			CZmq* pProxyZmq = GetThreadProxy();
			Assert(pProxyZmq);

			if (pProto && pProxyZmq)
			{
				if (!pProxyZmq->Send((void*)&sHeader, sizeof(sHeader), CZmq::HZMQ_SNDMORE))
					return false;

				if (!pProxyZmq->SendProtocol(pProto))
					return false;

				return true;		
			}
		}
		return false;
	}

	Bool BaseGateProxy::SendRawData(SID iSid, const OctetsStream* pData)
	{
		SvrMsgHeader sHeader;
		sHeader.Sid = iSid;
		return SendRawData(sHeader, pData);
	}

	Bool BaseGateProxy::SendRawData(const SvrMsgHeader& sHeader, const OctetsStream* pData)
	{
		if (m_iProxyState == UTIL_OK)
		{
			CZmq* pProxyZmq = GetThreadProxy();
			Assert(pProxyZmq);

			if (pData && pProxyZmq)
			{
				if (!pProxyZmq->Send((void*)&sHeader, sizeof(sHeader), CZmq::HZMQ_SNDMORE))
					return false;

				if (!pProxyZmq->Send(pData->AvailableData(), pData->Size()))
					return false;

				return true;
			}
		}
		return false;
	}

	Bool BaseGateProxy::BroadcastProto(const vector<SID>& vSid, CProtocol* pProto)
	{
		if (m_iProxyState == UTIL_OK && vSid.size() && pProto)
		{
			OctetsStream xOS;
			xOS << vSid;
			pProto->Encode(xOS);

			GateNotify sNotify(GateNotify::NOTIFY_PROTO_BROADCAST);			
			return SendNotify(sNotify, &xOS);
		}		
		return false;
	}

	Bool BaseGateProxy::SendNotify(const GateNotify& sNotify, const OctetsStream* pExtData)
	{
		CZmq* pProxyZmq = GetThreadProxy();
		Assert(pProxyZmq);

		if (pProxyZmq)
		{
			SvrMsgHeader sHeader;
			sHeader.Sid = 0;

			if (!pProxyZmq->Send((void*)&sHeader, sizeof(sHeader), CZmq::HZMQ_SNDMORE))
				return false;

			if (!pProxyZmq->Send((void*)&sNotify, sizeof(sNotify), pExtData ? CZmq::HZMQ_SNDMORE : 0))
				return false;

			if (pExtData && !pProxyZmq->Send((void*)pExtData->Begin(), pExtData->Size()))
				return false;

			return true;
		}

		return false;
	}

	Bool BaseGateProxy::SendRegsterMsg()
	{
		//先注册服务挂载
		if (m_iSvrId)
		{
			GateNotify sNotify(GateNotify::NOTIFY_SERVICE_ATTACH);
			sNotify.eAttach.SvrId     = m_iSvrId;
			sNotify.eAttach.ConnLimit = m_iConnLimit;
			SendNotify(sNotify);
		}

		//注册协议
		if (m_mProtoTypeDrive.size())
		{
			OctetsStream xOS;
			xOS << m_mProtoTypeDrive;

			GateNotify sNotify(GateNotify::NOTIFY_PROTO_REGISTER);
			SendNotify(sNotify, &xOS);
		}

		return true;
	}

	Bool BaseGateProxy::OnProxyConnected()
	{
		SendRegsterMsg();

		m_iProxyState = UTIL_OK;
		FmtPrint("Gateway Connect Successful.");

		return true;
	}

	Bool BaseGateProxy::OnProxyDisconnect()
	{
		m_iProxyState = UTIL_ERROR;
		FmtPrint("Gateway Disconnect Exception.");

		return true;
	}

	Bool BaseGateProxy::CheckProxyState()
	{
		if (m_pProxyZmq)
		{
			UInt32 iEvent = m_pProxyZmq->UpdateMonitor();
			if (iEvent)
			{
				if (iEvent & CZmq::HZMQ_EVENT_CONNECTED)
				{
					OnProxyConnected();
				}
				else if (iEvent & CZmq::HZMQ_EVENT_DISCONNECTED)
				{
					OnProxyDisconnect();
				}
			}
		}
		return m_iProxyState == UTIL_OK;
	}

	Bool BaseGateProxy::Update(Int32 iTimeout)
	{
		if (!CheckProxyState())
		{
			if (m_pCollectZmq)
				m_pCollectZmq->DiscardMsg();

			Sleep(DEFAULT_SLEEP);
			return true;
		}

		//接收网关来的协议并回调		
		Protocol* pProto = 0;
		GateMsgHeader sHeader;
		while (RecvProtocol(sHeader, pProto, iTimeout))
		{
			if (!OnRecvProtocol(sHeader, pProto))		
				g_ProtocolManager->ReleaseProto(pProto);

			//接收同时转发, 避免发送堆积
			g_ZmqManager->ProxyZmq(m_pCollectZmq, m_pProxyZmq, false, iTimeout, true);
		}

		//把服务器的协议转发给网关
		Bool bTransmit = g_ZmqManager->ProxyZmq(m_pCollectZmq, m_pProxyZmq, false, iTimeout, true);
		if (!bTransmit)
		{
			Assert(bTransmit && "GateProxy Transmit Error.");
		}		

		return true;
	}
}
