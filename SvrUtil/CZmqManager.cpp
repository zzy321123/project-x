#include "CZmqManager.h"
#include "OSOperator.h"
#include "zmq.h"

namespace Util
{
	SINGLETON_IMPL(ZmqManager);

	CZmqManager::CZmqManager()
	{
		m_pZmqCtx = 0;
	}

	CZmqManager::~CZmqManager()
	{
		ZmqMap::iterator it = m_mZmq.begin();
		for (;it != m_mZmq.end();++it)
		{
			CZmq* pZmq = it->second;

			if (pZmq)
				pZmq->Close();
			
			DEF_RELEASE(pZmq);
		}
		m_mZmq.clear();

		if(m_pZmqCtx)
		{
			zmq_ctx_destroy(m_pZmqCtx);
			m_pZmqCtx = 0;
		}
	}

	Bool CZmqManager::SetupZmqCtx(Int32 iThreads)
	{
		if (!m_pZmqCtx)
		{
			m_pZmqCtx = zmq_ctx_new();
			if (m_pZmqCtx)
			{
				zmq_ctx_set(m_pZmqCtx, ZMQ_IO_THREADS, iThreads);
			}
		}
		return m_pZmqCtx != 0;
	}

	void* CZmqManager::GetZmqCtx()
	{
		return m_pZmqCtx;
	}

	CZmq* CZmqManager::CreateZmq(Int32 iType)
	{
		if (m_pZmqCtx || SetupZmqCtx())
		{
			CZmq* pZmq = new CZmq();
			if (pZmq->Init(iType))
			{
				m_mZmq[pZmq] = pZmq;
				return pZmq;
			}
			else
			{
				DEF_RELEASE(pZmq);
			}
		}
		return 0;
	}

	Bool CZmqManager::CloseZmq(CZmq*& pZmq)
	{
		if (pZmq)
		{
			ZmqMap::iterator it = m_mZmq.find(pZmq);
			if (it != m_mZmq.end())
			{
				if (pZmq)
					pZmq->Close();

				m_mZmq.erase(it);
				DEF_RELEASE(pZmq);
				return true;
			}
		}
		return false;
	}

	Bool CZmqManager::ProxyZmq(CZmq* pFrontend, CZmq* pBackend, Bool bBothway, Int32 iTimeout, Bool bOnce)
	{
		Assert(pFrontend && pBackend);
		if (!pFrontend || !pBackend)
			return false;		

		zmq_pollitem_t items[] = 
		{
			{ pFrontend->GetHandle(), 0, 0, 0 },
			{ pBackend->GetHandle(),  0, 0, 0 }
		};
		
		items[0].events = ZMQ_POLLIN;
		if (bBothway)
			items[1].events = ZMQ_POLLIN;

		do
		{
			items[0].revents = 0;
			items[1].revents = 0;

			if (zmq_poll(items, 2, iTimeout) < 0)
				return false;

			if (items[0].revents & ZMQ_POLLIN) 
			{
				zmq_msg_t sMsg;
				if (zmq_msg_init(&sMsg) != UTIL_OK)
					return false;

				while (true) 
				{
					if (zmq_recvmsg(items[0].socket, &sMsg, 0) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					Int32  iRecvMore = 0;
					Size_t iLen = sizeof(iRecvMore);
					if (zmq_getsockopt(items[0].socket, ZMQ_RCVMORE, &iRecvMore, &iLen) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					if (zmq_sendmsg(items[1].socket, &sMsg, iRecvMore? ZMQ_SNDMORE : 0) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					if (iRecvMore == 0)
						break;
				}

				//ÊÍ·ÅÏûÏ¢
				zmq_msg_close(&sMsg);				
			}
		
			if (items[1].revents & ZMQ_POLLIN) 
			{
				zmq_msg_t sMsg;
				if (zmq_msg_init(&sMsg) != UTIL_OK)
					return false;

				while (true) 
				{
					if (zmq_recvmsg(items[1].socket, &sMsg, 0) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					Int32  iRecvMore = 0;
					Size_t iLen = sizeof(iRecvMore);
					if (zmq_getsockopt(items[1].socket, ZMQ_RCVMORE, &iRecvMore, &iLen) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					if (zmq_sendmsg(items[0].socket, &sMsg, iRecvMore? ZMQ_SNDMORE: 0) < 0)
					{
						zmq_msg_close(&sMsg);
						return false;
					}

					if (iRecvMore == 0)
						break;
				}

				zmq_msg_close(&sMsg);
			}
		}while(!bOnce);
		
		return true;
	}
}
