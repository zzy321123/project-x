#include "TimerManager.h"
#include "Atomic.h"

namespace Util
{
	SINGLETON_IMPL(TimerManager);

	CTimerManager::CTimerManager() : m_iTimer(0)
	{
		m_pMutex = new CMutex;		
		m_mTimer.clear();
	}

	CTimerManager::~CTimerManager()
	{		
		DEF_RELEASE(m_pMutex);

		TimerMap::iterator it = m_mTimer.begin();
		for (;it!=m_mTimer.end();it++)
		{
			UtilFree(it->second);
		}
		m_mTimer.clear();
	}

	Bool  CTimerManager::Tick(UInt32 iPeriod)
	{
		if (m_mTimer.size())
		{
			AutoMutex(lock, m_pMutex);

			TimerMap::iterator it = m_mTimer.begin();
			for (;it!=m_mTimer.end();)
			{
				if (!UpdateTimer(it->second, iPeriod))
				{
					UtilFree(it->second);
					m_mTimer.erase(it++);
					continue;
				}
				it ++;
			}
		}

		return CManagerBase::Tick(iPeriod);
	}

	Bool CTimerManager::UpdateTimer(Timer* pTimer, UInt32 iPeriod)
	{
		if (pTimer)
		{
			pTimer->Elapse += iPeriod;
			if (pTimer->Elapse >= pTimer->Period)
			{
				pTimer->Elapse = 0;
				pTimer->Count ++;

				if(pTimer->Entry)
					(*pTimer->Entry)(pTimer->Args, pTimer->TimerId, pTimer->Count);
			}

			if(!pTimer->Limit || pTimer->Count < pTimer->Limit)
				return true;
		}
		return false;		
	}

	UInt32 CTimerManager::AddTimer(UInt32 iPeriod, TimerEntry pEntry, void* pArgs, UInt32 iLimit)
	{		
		if (iPeriod && pEntry)
		{
			UInt32 iTimer = CAtomic::Inc<UInt32>(&m_iTimer);
			Timer* pTimer = (Timer*)UtilMalloc(sizeof(Timer));
			pTimer->TimerId	= iTimer;
			pTimer->Period	= iPeriod;
			pTimer->Limit	= iLimit;
			pTimer->Entry	= pEntry;
			pTimer->Args	= pArgs;
			pTimer->Count	= 0;
			pTimer->Elapse	= 0;

			AutoMutex(lock, m_pMutex);
			m_mTimer[iTimer] = pTimer;
			return pTimer->TimerId;
		}
		return 0;
	}

	Bool  CTimerManager::KillTimer(UInt32 iTimer)
	{
		AutoMutex(lock, m_pMutex);
		TimerMap::iterator it = m_mTimer.find(iTimer);
		if (it != m_mTimer.end())
		{
			UtilFree(it->second);
			m_mTimer.erase(it);
			return true;
		}
		return false;
	}
}
