#ifndef _TIMERMANAGER_H_
#define _TIMERMANAGER_H_

#include "ManagerBase.h"
#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* ��ʱ����������װ                                                     */
	/************************************************************************/
	class UTIL_API CTimerManager : public CManagerBase
	{
	protected:
		//����
		CTimerManager();

		//����
		virtual ~CTimerManager();

		//��������������
		SINGLETON_DECL(TimerManager);

	protected:
		//��ʱ���ص�������ʽ
		typedef void (*TimerEntry)(void* pArgs, UInt32 iTimer, UInt32 iCount);

		//��ʱ������
		struct Timer
		{
			UInt32	   TimerId;
			UInt32	   Period;
			UInt32	   Elapse;
			UInt32	   Limit;
			UInt32	   Count;
			void*	   Args;
			TimerEntry Entry;
		};

		//��ʱ����
		typedef map<UInt32, Timer*> TimerMap;

	public:
		//���¶�ʱ��
		virtual Bool   Tick(UInt32 iPeriod);

		//��Ӷ�ʱ��ʵ��
		virtual UInt32 AddTimer(UInt32 iPeriod, TimerEntry pEntry, void* pArgs = 0, UInt32 iLimit = 0);

		//ɾ����ʱ��ʵ��
		virtual Bool   KillTimer(UInt32 iTimer);

	protected:
		//���¶�ʱ������
		virtual Bool   UpdateTimer(Timer* sTimer, UInt32 iPeriod);

	protected:
		//��ʱ������
		CMutex*	m_pMutex;
		//��ʱ����
		TimerMap	m_mTimer;
		//��ǰ���䶨ʱ��ID
		UInt32		m_iTimer;
	};
	
	#define g_TimerManager CTimerManager::GetInstance()
}
#endif
