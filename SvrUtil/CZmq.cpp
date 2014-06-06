#include "CZmq.h"
#include "CZmqManager.h"
#include "OSOperator.h"
#include "LoggerManager.h"
#include "zmq.h"

namespace Util
{
	CZmq::CZmq()
	{		
		m_pHandle	= 0;
		m_pMonitor	= 0;
		m_iErrCode  = 0;
		m_iType		= 0;
		m_iRecvMore = 0;		 
		m_pRecvBuf  = 0;
		m_sErrMsg   = "";

#ifdef _DEBUG
		m_iLastSend = 0;
		m_iLastRecv = 0;
#endif
	}

	CZmq::~CZmq()
	{
		Close();

		DEF_RELEASE(m_pRecvBuf);
	}

	UInt64  CZmq::GenValidZid(UInt32 iTid)
	{
		return MAKE_UINT64(iTid, 1);
	}

	Int32 CZmq::GetErrCode() const
	{
		return m_iErrCode;
	}

	Bool CZmq::Init(Int32 iType)
	{
		A_Exception(m_pHandle == 0);

		m_iType   = iType;
		m_pHandle = zmq_socket(g_ZmqManager->GetZmqCtx(), m_iType);
		if (m_pHandle)
		{
			Int32 iLinger = 0;
			if (!SetOption(ZMQ_LINGER,  &iLinger, sizeof(iLinger)))
				return false;

			Int32 iTimeout = HZMQ_TIMEOUT;
			if (!SetOption(ZMQ_RCVTIMEO, &iTimeout, sizeof(iTimeout)) ||
				!SetOption(ZMQ_SNDTIMEO, &iTimeout, sizeof(iTimeout)) )
				return false;
		}

		return m_pHandle != 0;
	}

	void* CZmq::GetHandle()
	{
		return m_pHandle;
	}

	Int32 CZmq::GetType() const
	{
		return m_iType;
	}

	Bool CZmq::FillErr()
	{
		m_iErrCode = zmq_errno();

#ifdef _DEBUG
		m_sErrMsg  = zmq_strerror(m_iErrCode);

		FmtPrint("ZmqError, Errno: %d, Errmsg: %s", m_iErrCode, m_sErrMsg.c_str());
#endif

		return true;
	}

	Bool CZmq::Close()
	{
		m_iErrCode = UTIL_OK;
		if (m_pHandle)
		{
			Int32 iRet = zmq_close(m_pHandle);
			m_pHandle = 0;

			if(iRet == UTIL_ERROR)
				FillErr();
		}

		if (m_pMonitor)
		{
			Int32 iRet = zmq_close(m_pMonitor);
			m_pMonitor = 0;

			if(iRet == UTIL_ERROR)
				FillErr();
		}

		return m_iErrCode == UTIL_OK;
	}

	Bool CZmq::SetIdentity(const void* pOptVal, Int32 iSize)
	{
		Assert(iSize && iSize <= 255);
		Assert(*((const UChar*)pOptVal) != 0);

		if(pOptVal && iSize > 0)
		{
			return SetOption(ZMQ_IDENTITY, pOptVal, iSize);
		}

		return false;
	}

	Bool CZmq::SetOption(Int32 iOption, const void* pOptVal, Size_t iSize)
	{
		if (m_pHandle)
		{
			Int32 iRet = zmq_setsockopt(m_pHandle, iOption, pOptVal, iSize);
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return false;
			}
			return true;
		}
		return false;
	}

	Bool CZmq::GetOption(Int32 iOption,void* pOptVal,Size_t& iSize)
	{
		if (m_pHandle)
		{
			Int32 iRet = zmq_getsockopt(m_pHandle, iOption, pOptVal, &iSize);
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return false;
			}
			return true;
		}
		return false;
	}

	Bool CZmq::Bind(const AString& sAddr)
	{
		if (m_pHandle)
		{
			Int32 iRet = zmq_bind(m_pHandle, sAddr.c_str());
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return false;
			}
			return true;
		}
		return false;
	}

	Bool CZmq::Connect(const AString& sAddr)
	{
		if (m_pHandle)
		{
			Int32 iReconnIvl = 1000;
			if (!SetOption(ZMQ_RECONNECT_IVL, &iReconnIvl, sizeof(iReconnIvl)))
				return false;

			Int32 iRet = zmq_connect(m_pHandle,sAddr.c_str());
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return false;
			}
			return true;
		}
		return false;
	}		

	Bool CZmq::IsWaitRecv() const
	{
		return m_iRecvMore > 0;
	}

	UInt32 CZmq::PollEvent(UInt32 iEvents, Int32 iTimeout)
	{
		if (m_pHandle)
		{
			zmq_pollitem_t items[] = 
			{
				{ m_pHandle, 0, iEvents, 0 },
			};

			Int32 iRet = zmq_poll(items, 1, iTimeout);
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return 0;
			}
			return items[0].revents;
		}
		return 0;
	}

	Bool CZmq::StartMonitor(UInt32 iEvents, const AString& sAddr)
	{
		if (m_pHandle)
		{
			Long lMonitorId = (Long)m_pHandle;
			Char sMonitorAddr[DEFAULT_SIZE] = {0};
			
			if (sAddr.size())
				sprintf(sMonitorAddr, "%s", sAddr.c_str());
			else
				sprintf(sMonitorAddr, "inproc://project-monitor-%ld", lMonitorId);
			
			if (zmq_socket_monitor(m_pHandle, sMonitorAddr, (Int32)iEvents) == UTIL_ERROR)
				return false;

			if (!sAddr.size())
			{
				if (m_pMonitor)
				{
					zmq_close(m_pMonitor);
					m_pMonitor = 0;
				}

				m_pMonitor = zmq_socket(g_ZmqManager->GetZmqCtx(), HZMQ_PAIR);
				if (!m_pMonitor)
				{
					FillErr();
					return false;
				}

				Int32 iLinger = 0;
				zmq_setsockopt (m_pMonitor, ZMQ_LINGER, &iLinger, sizeof(iLinger));
				
				Int32 iTimeout = HZMQ_TIMEOUT;
				zmq_setsockopt (m_pMonitor, ZMQ_RCVTIMEO, &iTimeout, sizeof(iTimeout));
				zmq_setsockopt (m_pMonitor, ZMQ_SNDTIMEO, &iTimeout, sizeof(iTimeout));

				Int32 iRet = zmq_connect(m_pMonitor, sMonitorAddr);
				if (iRet == UTIL_ERROR)
				{
					FillErr();			
					return false;
				}
			}
			return true;
		}

		return false;
	}

	UInt32 CZmq::UpdateMonitor(OctetsStream* pData)
	{
		if (m_pMonitor)
		{
			zmq_pollitem_t items[] = 
			{
				{ m_pMonitor, 0, HEVENT_READ, 0 },
			};

			Int32 iRet = zmq_poll(items, 1, 0);
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return 0;
			}

			if (items[0].revents & HEVENT_READ)
			{
				zmq_event_t sEvent;
				Int32 iRecv = zmq_recv(m_pMonitor, &sEvent, sizeof(sEvent), 0);
				if(iRecv == UTIL_ERROR)
				{
					FillErr();
					return 0;
				}
				else if (iRecv == sizeof(sEvent))
				{
					if (pData)
						pData->Replace(&sEvent, sizeof(sEvent));

					return sEvent.event;
				}
			}
		}
		return 0;
	}

	Bool CZmq::StopMonitor()
	{
		if (m_pHandle)
		{
			if (m_pMonitor)
			{
				zmq_close(m_pMonitor);
				m_pMonitor = 0;
			}

			if (zmq_socket_monitor(m_pHandle, 0, 0) == UTIL_ERROR)
				return false;

			return true;
		}
		return false;
	}

	Bool CZmq::Send(void* pBuf, Size_t iSize, Int32 iFlag)
	{
		if (m_pHandle && pBuf && iSize)
		{
			Int32 iSend = zmq_send(m_pHandle, pBuf, iSize, iFlag);
			if(iSend < 0)
			{
				FillErr();
				return false;
			}

#ifdef _DEBUG
			Assert((Size_t)iSend == iSize);
			m_iLastSend = iSend;
#endif

			return true;
		}
		return false;
	}

	Bool CZmq::Recv(void* pBuf, Size_t& iSize, Int32 iFlag)
	{
		if (m_pHandle && pBuf && iSize)
		{
			m_iRecvMore = 0;
			Int32 iRecv = zmq_recv(m_pHandle, pBuf, iSize, iFlag);
			if (iRecv == UTIL_ERROR) 
			{
				FillErr();
				return false;
			}

			iSize = iRecv;

			Size_t iLen = sizeof(m_iRecvMore);
			Int32 iRet  = zmq_getsockopt(m_pHandle, ZMQ_RCVMORE, &m_iRecvMore, &iLen);
			if(iRet == UTIL_ERROR)
			{
				FillErr();
				return false;
			}

#ifdef _DEBUG
			m_iLastRecv = iRecv;
#endif

			return true;
		}
		return false;
	}

	Bool CZmq::DiscardMsg()
	{
		if(!m_pRecvBuf)
			m_pRecvBuf = new OctetsStream(RCVBUF_SIZE);

		while (PollEvent(HEVENT_READ, 0))
		{
			m_pRecvBuf->Clear();
			Size_t iSize = (Size_t)m_pRecvBuf->Capacity();
			if(!Recv(m_pRecvBuf->Begin(), iSize, 0))
				return false;
			
			while (IsWaitRecv())
			{
				m_pRecvBuf->Clear();
				Size_t iSize = (Size_t)m_pRecvBuf->Capacity();
				if (!Recv(m_pRecvBuf->Begin(), iSize, 0))
					return false;
			}
		}

		return true;
	}

	Bool CZmq::SendProtocol(CProtocol* pProto, Int32 iFlag)
	{
		if (pProto)
		{
			if(!m_pRecvBuf)
				m_pRecvBuf = new OctetsStream(RCVBUF_SIZE);

			if (m_pRecvBuf)
			{
				m_pRecvBuf->Clear();
				if(pProto->Encode(*m_pRecvBuf))
				{
					return Send(m_pRecvBuf->Begin(), (Size_t)m_pRecvBuf->Size(), iFlag);
				}
			}
		}
		return false;
	}

	Bool CZmq::RecvProtocol(CProtocol*& pProto, Int32 iFlag)
	{
		if(!m_pRecvBuf)
			m_pRecvBuf = new OctetsStream(RCVBUF_SIZE);

		if (m_pRecvBuf)
		{
			m_pRecvBuf->Clear();
			Size_t iSize = (Size_t)m_pRecvBuf->Capacity();
			if (Recv(m_pRecvBuf->Begin(), iSize, iFlag) && iSize)
			{
				m_pRecvBuf->Resize((UInt32)iSize);
				pProto = g_ProtocolManager->Decode(*m_pRecvBuf);
				if (pProto)
					return true;
			}
		}		
		return false;
	}
}
