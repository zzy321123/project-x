#include "Mutex.h"
#include "OSOperator.h"
#include "pthread.h"

namespace Util
{
	CMutex::Scope::Scope(CMutex* pMutex, UInt32 iFlag, const AString& sFile, Int32 iLine)
	{
		m_pMutex = pMutex;
		m_iFlag  = iFlag;

		if(m_pMutex && (m_iFlag & LOCK))
		{
			m_pMutex->Lock(sFile,iLine);
		}
	};

	CMutex::Scope::~Scope()
	{
		if(m_pMutex && (m_iFlag & UNLOCK))
		{
			m_pMutex->Unlock();
		}
	}

	//////////////////////////////////////////////////////////////////////////

	CMutex::CMutex(Bool bRecursive) : m_pMutex(0)
	{
		pthread_mutexattr_t sAttr;
		pthread_mutexattr_init(&sAttr);
		pthread_mutexattr_settype(&sAttr, bRecursive? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL);

		m_pMutex = UtilMalloc(sizeof(pthread_mutex_t));
		if (pthread_mutex_init((pthread_mutex_t*)m_pMutex, &sAttr) != UTIL_OK)
		{
			UtilFree(m_pMutex);
			m_pMutex = 0;
		}

		pthread_mutexattr_destroy(&sAttr);

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif
	}

	CMutex::~CMutex()
	{
		if (m_pMutex)
		{
			pthread_mutex_destroy((pthread_mutex_t*)m_pMutex);			
			UtilFree(m_pMutex);
			m_pMutex = 0;
		}
	}

	Bool CMutex::Lock(const AString& sFile, Int32 iLine)
	{
		Assert(m_pMutex);

		if (pthread_mutex_lock((pthread_mutex_t*)m_pMutex) == UTIL_OK)
		{
#ifdef _DEBUG
			m_bLocked = true;
			m_sFile   = sFile;
			m_iLine   = iLine;
			m_iThread = COSOperator::GetThreadId();
#endif
			return true;
		}
		return false;
	}

	Bool CMutex::TryLock(const AString& sFile,Int32 iLine)
	{
		Assert(m_pMutex);

		if (pthread_mutex_trylock((pthread_mutex_t*)m_pMutex) == UTIL_OK)
		{
#ifdef _DEBUG
			m_bLocked = true;
			m_sFile   = sFile;
			m_iLine   = iLine;
			m_iThread = COSOperator::GetThreadId();
#endif
			return true;
		}
		return false;
	}

	Bool CMutex::Unlock()
	{
		Assert(m_pMutex);

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif

		if (pthread_mutex_unlock((pthread_mutex_t*)m_pMutex) == UTIL_OK)
			return true;

		return false;
	}
}
