#include "Thread.h"
#include "OSOperator.h"
#include "pthread.h"

namespace Util
{
	PVoid fun_ThreadRoutine(void* pArgs)
	{
		CThread* pThread = static_cast<CThread*>(pArgs); 
		if (pThread)
			return pThread->RunLoop();

		return 0;
	}

	CThread::CThread(Bool bWaitTask) : m_pRoutine(0)
	{
		InnerInit();
	}

	CThread::CThread(CTask* pTask) : m_pRoutine(0)
	{
		InnerInit();

		AddTask(pTask);		
	}

	CThread::CThread(PThreadRoutine pRoutine) : m_pRoutine(pRoutine)
	{
		InnerInit();
	}	

	CThread::~CThread()
	{
		Close();

		if (m_pThread)
		{
			UtilFree(m_pThread);
			m_pThread = 0;
		}

		DEF_RELEASE(m_pLock);
	}

	Bool CThread::InnerInit()
	{
		m_pOwner		= 0;
		m_iId			= 0;
		m_pArgs			= 0;
		m_iPushTaskCnt	= 0;
		m_iPopTaskCnt	= 0;
		m_pThread		= 0;
		m_bRunning		= false;
		m_iState		= STATE_NONE;
		m_pLock  		= new CSpinLock;

		return true;
	}

	UInt64 CThread::GetPushTaskCnt() const
	{
		return m_iPushTaskCnt;
	}

	UInt64 CThread::GetPopTaskCnt() const
	{
		return m_iPopTaskCnt;
	}

	UInt32 CThread::GetThreadId() const
	{
		return m_iId;
	}

	Bool CThread::IsRunning() const
	{
		return m_bRunning;
	}

	Bool CThread::IsClosing() const
	{
		return m_iState == STATE_CLOSING;
	}

	void CThread::SetOwner(PVoid pPool)
	{
		m_pOwner = pPool;
	}

	PVoid CThread::GetOwner()
	{
		return m_pOwner;
	}

	Bool CThread::AddTask(CTask* pTask, Bool bFirst)
	{
		if(pTask)
		{		
			pTask->AddRef();
			AutoSpinLock(lock, m_pLock);

			if(bFirst)
				m_qTask.push_front(pTask);
			else
				m_qTask.push_back(pTask);

			m_iPushTaskCnt ++;
			return true;
		}
		return false;
	}

	Bool CThread::Start(void* pArgs, Bool bHighPriority)
	{
		if(!m_bRunning && !m_pThread)
		{			
			m_pArgs    = pArgs;
			m_bRunning = true;
			m_pThread  = UtilMalloc(sizeof(pthread_t));

			pthread_attr_t* pAttr = 0;

			pthread_attr_t sAttr;
			pthread_attr_init(&sAttr);
			pthread_attr_setschedpolicy(&sAttr,SCHED_RR);

			if (bHighPriority)
			{
				sched_param sParam; 
				memset(&sParam, 0, sizeof(sParam));
				sParam.sched_priority = sched_get_priority_max(SCHED_RR);
				pthread_attr_setschedparam(&sAttr, &sParam);
			}

			if (pthread_create((pthread_t*)m_pThread, pAttr, fun_ThreadRoutine, this) != UTIL_OK)
			{
				m_bRunning = false;
				m_iState   = STATE_CLOSED;
				pthread_attr_destroy(&sAttr);
				return false;
			}
			pthread_attr_destroy(&sAttr);

			while (m_iState != STATE_RUNNING)
			{
				Sleep(DEFAULT_SLEEP);
			}

			return true;
		}
		return false;
	}

	Bool CThread::Close()
	{
		if (m_pThread && m_bRunning) 
		{
			m_bRunning = false;
			m_iState   = STATE_CLOSING;

			//设置退出信号
			pthread_cancel(*((pthread_t*)m_pThread));
			//等待线程结束
			pthread_join(*((pthread_t*)m_pThread), 0);

			UtilFree(m_pThread);
			m_pThread = 0;
			m_iId     = 0;
			m_iState  = STATE_CLOSED;
		}

		ClearTaskQueue();
		return true;
	}

	void CThread::ClearTaskQueue()
	{
		AutoSpinLock(lock, m_pLock);
		TaskQueue::iterator it = m_qTask.begin();
		for (;it!=m_qTask.end();it++)
		{
			CTask* pTask = *it;
			if(pTask)
				pTask->Close();
			DEF_RELEASE(pTask);
		}
		m_qTask.clear();
	}

	void CThread::RunTaskQueue()
	{
		while (m_bRunning)
		{
			CTask* pTask = 0;
			if (m_qTask.size())
			{
				AutoSpinLock(lock, m_pLock);
				if (m_qTask.size())
				{
					pTask = m_qTask.front();
					m_qTask.pop_front();
					m_iPopTaskCnt ++;
				}
			}

			if (pTask)
			{
				pTask->Run(pTask->GetParam());
				pTask->Close();
				DEF_RELEASE(pTask);
			}
			else
			{
				break;
			}
		}
	}

	PVoid CThread::RunLoop()
	{
		m_iId	 = COSOperator::GetThreadId();
		m_iState = STATE_RUNNING;

		if (m_pRoutine)
		{
			return (*m_pRoutine)(m_pArgs);
		}

		while (m_bRunning)
		{
			CTask* pTask = 0;
			if (m_qTask.size())
			{
				AutoSpinLock(lock, m_pLock);
				if (m_qTask.size())
				{
					pTask = m_qTask.front();
					m_qTask.pop_front();
					m_iPopTaskCnt ++;
				}
			}

			if (pTask)
			{
				pTask->Run(pTask->GetParam());
				pTask->Close();
				DEF_RELEASE(pTask);
			}
			else
			{
				Sleep(DEFAULT_SLEEP);
			}
		}

		return 0;
	}
}
