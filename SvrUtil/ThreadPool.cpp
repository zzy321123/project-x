#include "ThreadPool.h"
#include "OSOperator.h"
#include "Atomic.h"

namespace Util
{
	PVoid fun_ThreadPoolRoutine(void* pData)
	{
		CThread* pThread = (CThread*)pData;
		if (!pThread || !pThread->GetOwner())
			return 0;

		CThreadPool* pPool = (CThreadPool*)pThread->GetOwner();
		while (pPool->IsRunning())
		{
			CTask* pTask = 0;
			if (!pPool->PopTask(&pTask))
				return false;

			if (pTask)
			{
				pTask->Run(pTask->GetParam());
				pTask->Close();
				DEF_RELEASE(pTask);
			}
		}
		return 0;
	}

	CThreadPool::CThreadPool()
	{
		m_bPoolQueue   = true;
		m_bRunning	   = false;
		m_iTurnIdx	   = 0;
		m_iThreadNum   = 0;
		m_iPushTaskCnt = 0;
		m_iPopTaskCnt  = 0;
		m_pLock		   = new CSpinLock;
	}

	CThreadPool::~CThreadPool()
	{
		Close();

		for (Size_t i=0;i<m_vThread.size();i++)
		{
			DEF_RELEASE(m_vThread[i]);
		}
		m_vThread.clear();

		DEF_RELEASE(m_pLock);
	}

	Bool CThreadPool::IsRunning() const
	{
		return m_bRunning;
	}

	UInt64 CThreadPool::GetPushTaskCnt() const
	{
		return m_iPushTaskCnt;
	}

	UInt64 CThreadPool::GetPopTaskCnt() const
	{
		return m_iPopTaskCnt;
	}

	Int32 CThreadPool::GetThreadNum()
	{
		return m_iThreadNum;
	}

	Int32 CThreadPool::GetThreadId(Int32 iIdx)
	{
		if (iIdx >= 0 && iIdx < m_iThreadNum)
		{
			return m_vThread[iIdx]->GetThreadId();
		}
		return 0;
	}

	Bool CThreadPool::InitPool(Int32 iPoolSize, Bool bPoolQueue)
	{
		m_bPoolQueue = bPoolQueue;
		for (Int32 i = 0; i < iPoolSize; i++)
		{
			CThread* pThread = 0;
			if (m_bPoolQueue)
				pThread = new CThread(fun_ThreadPoolRoutine);
			else
				pThread = new CThread(true);
			
			if (pThread)
			{
				pThread->SetOwner(this);
				m_vThread.push_back(pThread);
			}
		}
		m_iThreadNum = (Int32)m_vThread.size();
		return true;
	}

	Bool CThreadPool::Start()
	{
		if (!m_bRunning)
		{	
			m_bRunning = true;
			for (Size_t i = 0; i < m_vThread.size(); i++)
			{
				m_vThread[i]->Start(m_vThread[i]);
			}
			return true;
		}
		return false;
	}

	Bool CThreadPool::AddTask(CTask* pTask, Int32 iThreadIdx, Bool bFirst)
	{
		if (!m_bRunning || !pTask || !m_iThreadNum)
			return false;

		if (m_bPoolQueue)
		{
			pTask->AddRef();

			AutoSpinLock(lock, m_pLock);
			if(bFirst)
				m_qTask.push_front(pTask);
			else
				m_qTask.push_back(pTask);

			m_iPushTaskCnt ++;
		}
		else
		{		
			if (iThreadIdx < 0)
				iThreadIdx = CAtomic::Inc<UInt32>(&m_iTurnIdx);
			else if (iThreadIdx >= m_iThreadNum)
				iThreadIdx = iThreadIdx % m_iThreadNum;

			if (iThreadIdx >= 0 && iThreadIdx < m_iThreadNum)
			{
				return m_vThread[iThreadIdx]->AddTask(pTask, bFirst);
			}
		}
		return true;
	}

	void CThreadPool::Close()
	{
		m_bRunning = false;
		for (Size_t i = 0; i < m_vThread.size(); i++)
		{
			m_vThread[i]->Close();
		}
	}

	Bool CThreadPool::PopTask(CTask** ppTask)
	{
		if (m_bRunning && ppTask)
		{
			*ppTask = 0;
			if (m_qTask.size())
			{
				AutoSpinLock(lock, m_pLock);
				if (m_qTask.size())
				{
					*ppTask = m_qTask.front();
					m_qTask.pop_front();
					m_iPopTaskCnt ++;
				}				
			}

			if (*ppTask == 0)
			{
				Sleep(DEFAULT_SLEEP);
			}

			return true;
		}
		return false;
	}
}
