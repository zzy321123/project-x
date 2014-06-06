#include "BaseGateThread.h"
#include "BaseGateway.h"
#include "event.h"
#include "zmq.h"

namespace Util
{
	//线程入口函数
	PVoid fun_GateThreadRoutine(void* pArgs)
	{
		BaseGateThread* pThread = (BaseGateThread*)pArgs;
		Assert(pThread);
		if (pThread)
			pThread->OnThreadLoop();

		return 0;
	}

	//会话读事件处理函数
	void fun_GateSessionRead(bufferevent* pEvent, void* pArgs) 
	{
		BaseGateThread::Session* pSession = (BaseGateThread::Session*)pArgs;
		Assert(pSession);
		if (pSession && pSession->GThread)
			pSession->GThread->OnSessionEvent(HEVENT_READ, pSession);
	}

	//会话写事件处理函数
	void fun_GateSessionWrite(bufferevent* pEvent, void* pArgs) 
	{
		BaseGateThread::Session* pSession = (BaseGateThread::Session*)pArgs;
		Assert(pSession);
		if (pSession && pSession->GThread)
			pSession->GThread->OnSessionEvent(HEVENT_WRITE, pSession);
	}

	//会话错误事件处理函数
	void fun_GateSessionError(bufferevent* pEvent, Short iWhat, void* pArgs) 
	{
		BaseGateThread::Session* pSession = (BaseGateThread::Session*)pArgs;
		Assert(pSession);
		if (pSession && pSession->GThread)
			pSession->GThread->OnSessionEvent(HEVENT_EXCEPT, pSession, iWhat);
	}

	//////////////////////////////////////////////////////////////////////////

	BaseGateThread::BaseGateThread(BaseGateway* pGateway)
	{
		m_pThread	= 0;
		m_pBase		= 0;
		m_pZmq		= 0;
		m_iBaseSid	= 0;
		m_iSidStep	= 0;
		m_iCurSid	= 0;
		m_pOctets	= 0;
		m_bIdle	    = true;
		m_bRunning  = false;
		m_pGateway	= pGateway;
	}

	BaseGateThread::~BaseGateThread()
	{
		//释放会话列表
		FreeSessionMap();

		//删除基础事件
		if (m_pBase)
		{
			event_base_free((event_base*)m_pBase);
			m_pBase = 0;
		}

		//关闭消息队列
		g_ZmqManager->CloseZmq(m_pZmq);

		//释放消息缓冲
		DEF_RELEASE(m_pOctets);

		//释放线程和锁
		DEF_RELEASE(m_pThread);		
	}

	Bool BaseGateThread::Init(UInt32 iIdentify, UInt32 iBaseSid, UInt32 iSidStep)
	{
		Assert(!m_pThread && !m_pBase && !m_pZmq);

		m_iBaseSid = iBaseSid + iIdentify;
		m_iSidStep = iSidStep;

		//创建通用缓冲
		if (!m_pOctets)
			m_pOctets = new OctetsStream(m_pGateway->GetBufSize());

		//创建线程
		if (!m_pThread)
			m_pThread = new CThread(fun_GateThreadRoutine);

		//创建事件基础对象
		if (!m_pBase)
		{
			event_config* pCfg = event_config_new();
			if (!pCfg) 
			{
				Print("Create EventConfig Failed.");
				return false;
			}

#ifdef PLATFORM_LINUX
			event_config_require_features(pCfg, EV_FEATURE_ET);
#endif
			event_config_set_flag(pCfg, EVENT_BASE_FLAG_NOLOCK);

			m_pBase = (void*)event_base_new_with_config(pCfg);
			event_config_free(pCfg);
			if (!m_pBase)
			{
				Print("Create EventBase Failed.");
				return false;
			}

			static Bool show_kernel_event_mechanism = true;
			if (show_kernel_event_mechanism)
			{
				show_kernel_event_mechanism = false;

				const Char* pszMethod = event_base_get_method((event_base*)m_pBase);
				if (pszMethod && strlen(pszMethod))
				{
					FmtPrint("Kernel Event Notification Mechanism: %s", pszMethod);
				}
			}			
		}

		//创建ZMQ对象
		if (!m_pZmq)
		{
			m_pZmq = g_ZmqManager->CreateZmq(CZmq::HZMQ_DEALER);

			UInt64 iZmqId = CZmq::GenValidZid(iIdentify);
			m_pZmq->SetIdentity(&iZmqId, (Int32)sizeof(iZmqId));

			m_pZmq->Connect(m_pGateway->GetThreadZmqAddr());
		}

		return true;
	}

	Bool BaseGateThread::Start()
	{
		if (m_pThread && m_pBase && m_pZmq && !m_bRunning)
		{
			m_bRunning = true;

			if (m_pThread)
				m_pThread->Start(this);

			return true;
		}
		return false;
	}

	Bool BaseGateThread::Close()
	{
		//等待线程结束
		if (m_pThread)
			m_pThread->Close();

		//释放当前连接
		FreeSessionMap();

		return true;
	}	

	Int32 BaseGateThread::GetThreadId() const
	{
		if (m_pThread)
			return m_pThread->GetThreadId();

		return 0;
	}

	SID  BaseGateThread::GenSessionId()
	{
		Assert(m_pGateway);		
		SID iSid = m_iCurSid++ * m_iSidStep + m_iBaseSid;
		return iSid;
	}

	BaseGateThread::Session* BaseGateThread::AllocSession(SOCKET hSocket, const SocketAddr& sAddr)
	{
		if (hSocket != INVALID_SOCKET)
		{			
			Session* pSession = (Session*)UtilMalloc(sizeof(Session));
			memset(pSession, 0, sizeof(Session));

			pSession->GThread		= this;
			pSession->Socket		= hSocket;
			pSession->Trust		= false;			
			pSession->Sid			= GenSessionId();
			pSession->State		= SESSION_STATE_CLOSED;
			pSession->Addr			= new SocketAddr(sAddr);
			pSession->IBuffer		= new OctetsStream(m_pGateway->GetBufSize());			
			pSession->ProtoTypeCrc = new ProtoCrcMap;

			CSocket::SetBlocking(pSession->Socket, false);
			m_pGateway->CreateISecurity(pSession->ISecurity);
			m_pGateway->CreateOSecurity(pSession->OSecurity);

			return pSession;
		}
		return 0;
	}

	Bool BaseGateThread::FreeSession(Session* pSession)
	{
		if (pSession)
		{
			//释放事件
			if (pSession->Event)
			{
				bufferevent_free((bufferevent*)pSession->Event);
				pSession->Event = 0;
			}

			//释放内存
			pSession->ProtoTypeCrc->clear();
			DEF_DELETE(pSession->ProtoTypeCrc);
			DEF_RELEASE(pSession->Addr);
			DEF_RELEASE(pSession->IBuffer);
			DEF_RELEASE(pSession->ISecurity);
			DEF_RELEASE(pSession->OSecurity);			

			//释放会话
			UtilFree(pSession);

			return true;
		}
		return false;
	}

	Bool BaseGateThread::FreeSessionMap()
	{
		SessionMap::iterator it = m_mSession.begin();
		for (;it!=m_mSession.end();it++)
		{
			Session* pSession = it->second;
			FreeSession(pSession);
		}
		m_mSession.clear();
		return true;
	}

	Bool BaseGateThread::OnThreadLoop()
	{
		if (m_pBase)
		{
			//日志记录
			FmtPrint("GateThread EventLoop, ThreadId: %u", COSOperator::GetThreadId());	

			while (m_bRunning)
			{
				//设置默认空闲状态
				m_bIdle = true;

				//开始消息循环
				event_base_loop((event_base*)m_pBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);

				//获取网关发送过来的数据消息
				OnGatewayEvent();

				//空闲状态
				OnThreadIdle();
			}
			return true;
		}
		return false;
	}

	Bool BaseGateThread::StartSession(SOCKET hSocket, const SocketAddr& sAddr, Session** ppSession)
	{
		//套接字有效
		if (hSocket == INVALID_SOCKET)
			return false;

		//创建缓存事件
		bufferevent* pEvent = bufferevent_socket_new((event_base*)m_pBase, hSocket, BEV_OPT_CLOSE_ON_FREE);
		if (!pEvent)
			return false;

		//创建会话
		Session* pSession = AllocSession(hSocket, sAddr);
		if (!pSession)
		{
			bufferevent_free(pEvent);
			return false;
		}
		pSession->Event = pEvent;
		pSession->State = SESSION_STATE_ACTIVE;

		//设置回调
		bufferevent_setcb(pEvent, fun_GateSessionRead, fun_GateSessionWrite, fun_GateSessionError, pSession);

		//设置读超时, 可做为心跳机制
		Int32 iTimeout = m_pGateway->GetSessionTimeout();
		if (iTimeout > 0)
		{
			struct timeval tv;
			tv.tv_sec  = iTimeout / 1000;
			tv.tv_usec = (iTimeout % 1000) * 1000; 
			bufferevent_set_timeouts(pEvent, &tv, NULL);
		}
		//开启读写事件
		bufferevent_enable(pEvent, EV_READ | EV_WRITE);

		//添加到会话列表
		m_mSession[pSession->Sid] = pSession;
		if (ppSession)
			*ppSession = pSession;

		//调用性能监视器
		if (m_pGateway->GetProfiler())
			m_pGateway->GetProfiler()->RegConnect(true);		

		//添加日志
		FmtLog("SessionStart, Sid: %u, Address: %s", pSession->Sid, pSession->Addr->ToString().c_str());				

		return true;
	}

	Bool BaseGateThread::CloseSession(SID iSid, Int32 iErrCode)
	{
		if (iSid)
		{
			SessionMap::iterator it = m_mSession.find(iSid);
			if (it != m_mSession.end())
			{
				//剔除会话列表
				Session* pSession = it->second;
				m_mSession.erase(it);

				//添加日志
				if (iErrCode)
					FmtLog("SessionClose, Sid: %u, Address: %s, ErrCode: %d, SocketErr: %d", pSession->Sid, pSession->Addr->ToString().c_str(), iErrCode, CSocket::GetSocketErr());
				else
					FmtLog("SessionClose, Sid: %u, Address: %s", pSession->Sid, pSession->Addr->ToString().c_str());

				//释放会话
				FreeSession(pSession);

				//调用性能监视器
				if (m_pGateway->GetProfiler())
					m_pGateway->GetProfiler()->RegConnect(false);				

				return true;
			}
		}		
		return false;
	}

	Bool BaseGateThread::RecvGateMsg(GateMsgHeader& sHeader, OctetsStream* pOctets)
	{
		if (m_pZmq && pOctets)
		{
			//接收SID头信息
			pOctets->Clear();
			Size_t iSize = (Size_t)pOctets->Capacity();
			if (!m_pZmq->Recv(pOctets->Begin(), iSize))
				return false;

			sHeader = *((GateMsgHeader*)pOctets->Begin());
			Bool bRecvMore = m_pZmq->IsWaitRecv();			
			Assert(iSize == sizeof(sHeader) && bRecvMore);
			if (iSize != sizeof(sHeader) || !bRecvMore)
				return false;

			//接收数据体
			pOctets->Clear();
			iSize = (Size_t)pOctets->Capacity();
			if (!m_pZmq->Recv(pOctets->Begin(), iSize))
				return false;

			pOctets->Resize((UInt32)iSize);
			return true;
		}
		return false;
	}

	Bool BaseGateThread::RecvGateExtData(OctetsStream* pOctets)
	{
		if (m_pZmq && pOctets)
		{
			//消息状态校验
			Bool bRecvMore = m_pZmq->IsWaitRecv();
			Assert(bRecvMore);
			if (!bRecvMore)
				return false;

			//接收数据体
			pOctets->Clear();
			Size_t iSize = (Size_t)pOctets->Capacity();
			if (!m_pZmq->Recv(pOctets->Begin(), iSize))
				return false;

			pOctets->Resize((UInt32)iSize);
			return true;
		}
		return false;
	}

	Bool BaseGateThread::SendGateMsg(const ThreadMsgHeader& sHeader, void* pData, Size_t iSize)
	{
		if (m_pZmq)
		{			
			//发送消息头
			if (!m_pZmq->Send((void*)&sHeader, sizeof(sHeader), CZmq::HZMQ_SNDMORE))
				return false;

			//发送数据体
			if (!m_pZmq->Send(pData, iSize))
				return false;

			return true;
		}
		return false;
	}

	Bool BaseGateThread::SendProtocol(Session* pSession, Protocol* pProto)
	{
		if (pSession && pProto)
		{
			//加密协议到缓冲区
			m_pOctets->Clear();
			pProto->Encode(*m_pOctets);

			if (OnSessionEncode(pSession, m_pOctets))
			{
				//写进缓冲区
				if (bufferevent_write((bufferevent*)pSession->Event, m_pOctets->Begin(), m_pOctets->Size()) != UTIL_OK)
				{
					FmtPrint("Event Buffer Write Error, Size: %u", m_pOctets->Size());
					return false;
				}

				//调用性能监视器
				if (m_pGateway->GetProfiler())
					m_pGateway->GetProfiler()->RegSendProto(pProto->GetType(), m_pOctets->Size());

				return true;
			}
		}
		return false;
	}

	Bool BaseGateThread::OnGatewayEvent()
	{
		while (m_pZmq && m_pZmq->PollEvent(HEVENT_READ, 0))
		{
			//设置非空闲
			m_bIdle = false;

			//接收网关发送的数据
			GateMsgHeader sHeader;
			if (!RecvGateMsg(sHeader, m_pOctets))
				return false;

			//会话协议转发
			if (sHeader.Sid)
			{
				SessionMap::iterator it = m_mSession.find(sHeader.Sid);
				if (it != m_mSession.end() && it->second)
				{
					Session* pSession = it->second;

					//提取协议基础数据
					ProtoType iProtoType = *((ProtoType*)m_pOctets->Begin());
					ProtoSize iProtoSize = 0;
					ProtoCrc  iProtoCrc  = 0;
					g_ProtocolManager->AnalyseProtocolHeader(*m_pOctets, iProtoType, iProtoSize, iProtoCrc);

					//状态驱动协议过滤
					Bool bStateProtoFilter = false;
					ProtoTypeDriveMap::const_iterator ptd_it = m_mProtoTypeDrive.find(iProtoType);
					if (pSession->ProtoTypeCrc && ptd_it != m_mProtoTypeDrive.end() && ptd_it->second == Protocol::DRIVE_STATE)
					{
						ProtoCrcMap::const_iterator pc_it = pSession->ProtoTypeCrc->find(iProtoType);
						if (pc_it != pSession->ProtoTypeCrc->end() && pc_it->second == iProtoCrc)
							bStateProtoFilter = true;
						else
							(*pSession->ProtoTypeCrc)[iProtoType] = iProtoCrc;
					}

					//加密输出缓冲区
					if (!bStateProtoFilter && OnSessionEncode(pSession, m_pOctets))
					{
						//写进缓冲区
						if (bufferevent_write((bufferevent*)pSession->Event, m_pOctets->Begin(), m_pOctets->Size()) != UTIL_OK)
						{
							FmtPrint("Event Buffer Write Error, Size: %u", m_pOctets->Size());
							OnSessionError(pSession, SE_BEV_WRITE);
							return false;
						}

						//调用性能监视器
						if (m_pGateway->GetProfiler())
							m_pGateway->GetProfiler()->RegSendProto(iProtoType, m_pOctets->Size());
					}
				}
			}
			//系统消息通知
			else
			{
				Assert(m_pOctets->Size() == sizeof(GateNotify));
				GateNotify* pNotify = (GateNotify*)m_pOctets->Begin();
				if (m_pOctets->Size() != sizeof(GateNotify))
					return false;

				//创建会话连接
				if (pNotify->Type == GateNotify::NOTIFY_SESSION_CONNECT)
				{
					if (pNotify->eConnect.Handle != INVALID_SOCKET && pNotify->eConnect.AddrLen)
					{
						Session* pSession = 0;
						SocketAddr sAddr((sockaddr*)pNotify->eConnect.Address, pNotify->eConnect.AddrLen);
						if (!StartSession(pNotify->eConnect.Handle, sAddr, &pSession))
						{
							closesocket(pNotify->eConnect.Handle);
						}
						else
						{
							//保存绑定的服务器ID
							pSession->SvrId = pNotify->eConnect.SvrId;

							//通知后端服务会话开启
							SysProtocol::Sys_SessionStart sProto(pSession->Sid, sAddr.ToString());
							m_pOctets->Clear();
							sProto.Encode(*m_pOctets);
							SendGateMsg(ThreadMsgHeader(pSession->SvrId, pSession->Sid, 0, sProto.GetType()), m_pOctets->Begin(), m_pOctets->Size());
						}
					}
				}
				//关闭会话连接
				else if (pNotify->Type == GateNotify::NOTIFY_SESSION_CLOSE)
				{
					Session* pSession = 0;
					SessionMap::iterator it = m_mSession.find(pNotify->eClose.Sid);					
					if (it != m_mSession.end())
						pSession = it->second;

					if (pSession)
					{
						Assert(pSession->SvrId == pNotify->eClose.SvrId);
						struct evbuffer* pBuf = bufferevent_get_output((bufferevent*)pSession->Event);
						if (!pBuf || !evbuffer_get_length(pBuf)) 
							CloseSession(pNotify->eClose.Sid);
						else
							pSession->State = SESSION_STATE_CLOSING;
					}					
				}
				//后端服务卸载, 默认关闭会话
				else if (pNotify->Type == GateNotify::NOTIFY_SERVICE_DETACH)
				{
					vector<SID> vCloseSid;
					//筛选处在当前关闭服务下的会话
					SessionMap::const_iterator it = m_mSession.begin();
					for (;it != m_mSession.end(); it++)
					{
						if (it->second->SvrId == pNotify->eDetach.SvrId)
							vCloseSid.push_back(it->first);
					}

					//统一关闭会话
					for (Size_t i=0;i<vCloseSid.size();i++)
					{
						CloseSession(vCloseSid[i], SE_SHUTDOWN);
					}				
				}
				//退出网关服务
				else if (pNotify->Type == GateNotify::NOTIFY_SERVICE_EXIT)
				{
					//设置退出标记
					m_bRunning = false;
					FmtPrint("Break EventLoop Success, ThreadId: %u", COSOperator::GetThreadId());
				}
				//协议注册
				else if (pNotify->Type == GateNotify::NOTIFY_PROTO_REGISTER)
				{
					//通知消息携带参数
					if (RecvGateExtData(m_pOctets))
					{
						map<ProtoType, UInt8> mTypeDrive;
						*m_pOctets >> mTypeDrive;

						map<ProtoType, UInt8>::iterator it = mTypeDrive.begin();
						for (;it!=mTypeDrive.end();it++)
							m_mProtoTypeDrive[ it->first ] = it->second;

						//系统协议
						vector<ProtoType> vSysType;
						g_ProtocolManager->GetRegProtoIds(vSysType);
						for (Size_t i=0; i<vSysType.size(); i++)
							m_mProtoTypeDrive[ vSysType[i] ] = Protocol::DRIVE_EVENT;
					}
				}
			}
		}
		return true;
	}

	Bool BaseGateThread::OnThreadIdle()
	{
		//空闲处理
		if (m_bIdle)
		{
			Sleep(DEFAULT_SLEEP);
		}		
		return true;
	}

	Bool BaseGateThread::OnSessionEvent(UInt32 iEvent, Session* pSession, Int32 iErrCode)
	{
		if (iEvent && pSession && pSession->State != SESSION_STATE_CLOSED)
		{
			//设置非空闲
			m_bIdle = false;

			//会话读事件
			if (iEvent & HEVENT_READ)
				OnSessionRead(pSession);

			//会话写事件
			if (iEvent & HEVENT_WRITE)
				OnSessionWrite(pSession);

			//会话异常事件
			if (iEvent & HEVENT_EXCEPT)
				OnSessionError(pSession, iErrCode);

			return true;
		}
		return false;
	}

	Bool BaseGateThread::OnSessionRead(Session* pSession)
	{
		if (!pSession || pSession->Socket == INVALID_SOCKET || pSession->State != SESSION_STATE_ACTIVE)
			return false;		

		//检测输入缓冲区
		struct evbuffer* pBuf = bufferevent_get_input((bufferevent*)pSession->Event);
		if (!pBuf || !evbuffer_get_length(pBuf)) 
		{
			OnSessionError(pSession, SE_BEV_LENGTH);
			return false;
		}

		//循环操作避免事件边缘触发引起的未读取问题
		while (evbuffer_get_length(pBuf))
		{
			m_pOctets->Clear();

			//读取数据
			Size_t iBufCap  = (Size_t)m_pGateway->GetBufSize();
			Assert(m_pOctets->Capacity() >= iBufCap);

			Int32 iReadSize = evbuffer_remove(pBuf, m_pOctets->Begin(), iBufCap);
			if (iReadSize <= 0)
			{
				OnSessionError(pSession, SE_BEV_REMOVE);
				return false;
			}
			m_pOctets->Resize(iReadSize);

			//调用性能监视器(只记流量, 解密前才是真实流量数据)
			if (m_pGateway->GetProfiler())
				m_pGateway->GetProfiler()->RegRecvProto(0, iReadSize);

			//解密
			if (!OnSessionDecode(pSession, m_pOctets))
			{
				OnSessionError(pSession, SE_DECIPHER);
				return false;
			}			

			//填充到输入缓冲区
			UInt32 iFillPos = 0;
			UInt32 iBufSize = m_pOctets->Size();
			while(iBufSize)
			{
				//尽可能填充解码缓冲区
				UInt32 iFillSize = CMath::Min<UInt32>(pSession->IBuffer->EmptyCap(), iBufSize);
				if (iFillSize)
				{
					void* pData = (Char*)m_pOctets->Begin() + iFillPos;
					pSession->IBuffer->Insert(pSession->IBuffer->End(), pData, iFillSize);
					iFillPos += iFillSize;
					iBufSize -= iFillSize;
				}
				//输入缓冲区已满, 出错关闭会话
				else
				{
					FmtError("IBuffer Full, BufCap: %u, ReadSize: %u", pSession->IBuffer->Capacity(), iBufSize);
					OnSessionError(pSession, SE_BUF_FULL);
					return false;
				}

				//网关信任检测
				if (!pSession->Trust)
				{
					if (!m_pGateway->TrustSession(pSession, pSession->IBuffer))
						break;

					pSession->Trust = true;

					//移除输入缓冲的前段空白
					pSession->IBuffer->RemoveBlank();
				}

				//从缓冲区中解码协议
				if (!DecodeIBuffer(pSession))
				{
					OnSessionError(pSession, SE_DECODE);
					return false;
				}
			}
		}
		return true;
	}

	Bool  BaseGateThread::DecodeIBuffer(Session* pSession)
	{
		if (pSession && pSession->IBuffer && pSession->IBuffer->Size())
		{
			//协议解码检测				
			ProtoType iProtoType = 0;
			ProtoSize iProtoSize = 0;
			ProtoCrc  iProtoCrc  = 0;
			//分析协议头
			while (g_ProtocolManager->AnalyseProtocolHeader(*pSession->IBuffer, iProtoType, iProtoSize, iProtoCrc))
			{
				//大小不合法
				if (!g_ProtocolManager->CheckSizeLimit(iProtoSize))
				{
					FmtError("Protocol Illicit, ProtoType: %u, ProtoSize: %u", iProtoType, iProtoSize);
					return false;
				}

				//检测协议合法性
				if (m_mProtoTypeDrive.size() && m_mProtoTypeDrive.find(iProtoType) == m_mProtoTypeDrive.end())
				{
					FmtError("Protocol Illicit, ProtoType: %d", iProtoType);
					return false;
				}

				UInt32 iChunkSize = 0;
				//协议包是否完全读取进来
				if (!g_ProtocolManager->CheckDecodeProtocol(*pSession->IBuffer, &iChunkSize))
					break;					

				//响应连接的Ping协议
				if (iProtoType == SysProtocol::SYS_CLT_PING)
				{
					//回复Pong响应
					SysProtocol::Sys_SvrPong sProto((UInt32)COSOperator::GetSysTime());
					if (!SendProtocol(pSession, &sProto))
						return false;
				}
				//传递给网关, 再发往后端服务器
				else
				{
					//给网关发协议消息
					SendGateMsg(ThreadMsgHeader(pSession->SvrId, pSession->Sid, 0, iProtoType), pSession->IBuffer->AvailableData(), iChunkSize);
				}

				//移动数据游标
				pSession->IBuffer->MoveNonius(iChunkSize);

				//调用性能监视器(只记协议数, 避免流量统计不准确)
				if (m_pGateway->GetProfiler())
					m_pGateway->GetProfiler()->RegRecvProto(iProtoType, 0);
			}			

			//移除输入缓冲的前段空白
			pSession->IBuffer->RemoveBlank();
		}
		return true;
	}

	Bool BaseGateThread::OnSessionWrite(Session* pSession)
	{
		if (!pSession || pSession->Socket == INVALID_SOCKET)
			return false;

		if (pSession->State == SESSION_STATE_CLOSING)
		{
			struct evbuffer* pBuf = bufferevent_get_output((bufferevent*)pSession->Event);
			if (!pBuf || !evbuffer_get_length(pBuf)) 
				CloseSession(pSession->Sid);
		}

		return true;
	}

	Bool BaseGateThread::OnSessionError(Session* pSession, Int32 iErrCode)
	{
		if (pSession)
		{
			//发送会话断开协议
			if (pSession->Sid)
			{
				SysProtocol::Sys_SessionBreak sProto(pSession->Sid, pSession->Addr->ToString());
				m_pOctets->Clear();
				sProto.Encode(*m_pOctets);
				SendGateMsg(ThreadMsgHeader(pSession->SvrId, pSession->Sid, 0, sProto.GetType()), m_pOctets->Begin(), m_pOctets->Size());
			}

			//关闭会话
			CloseSession(pSession->Sid, iErrCode);
			return true;
		}
		return false;
	}

	Bool BaseGateThread::OnSessionDecode(Session* pSession, OctetsStream* pBuffer)
	{
		if (pSession && pBuffer && pSession->ISecurity)
		{
			return pSession->ISecurity->Update(*pBuffer);
		}

		return true;
	}

	Bool BaseGateThread::OnSessionEncode(Session* pSession, OctetsStream* pBuffer)
	{
		if (pSession && pBuffer && pSession->OSecurity)
		{
			return pSession->OSecurity->Update(*pBuffer);
		}

		return true;
	}
}
