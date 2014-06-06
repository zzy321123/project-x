#include "RWLock.h"
#include "pthread.h"

namespace Util
{
	CRWLock::Scope::Scope(CRWLock* pRWLock, UInt32 iType, const AString& sFile, Int32 iLine) : m_pRWLock(pRWLock)
	{
		if (m_pRWLock)
			m_pRWLock->Lock(iType, sFile, iLine);
	}

	CRWLock::Scope::~Scope()
	{
		if (m_pRWLock)
			m_pRWLock->Unlock();
	}

	//////////////////////////////////////////////////////////////////////////

	CRWLock::CRWLock() : m_pRWLock(0)
	{
		m_pRWLock = UtilMalloc(sizeof(pthread_rwlock_t));
		if (pthread_rwlock_init((pthread_rwlock_t*)m_pRWLock,0) != UTIL_OK)
		{
			UtilFree(m_pRWLock);
			m_pRWLock = 0;
		}
	}

	CRWLock::~CRWLock()
	{
		if (m_pRWLock)
		{
			pthread_rwlock_destroy((pthread_rwlock_t*)m_pRWLock);
			UtilFree(m_pRWLock);
			m_pRWLock = 0;
		}
	}

	Bool CRWLock::Lock(UInt32 iType, const AString& sFile, Int32 iLine)
	{
		Assert(m_pRWLock);

		if (iType == READ)
		{
			if (pthread_rwlock_rdlock((pthread_rwlock_t*)m_pRWLock) == UTIL_OK)
				return true;
		}
		else if (iType == WRITE)
		{
			if (pthread_rwlock_wrlock((pthread_rwlock_t*)m_pRWLock) == UTIL_OK)
				return true;
		}
		return false;
	}

	Bool CRWLock::TryLock(UInt32 iType, const AString& sFile,Int32 iLine)
	{
		Assert(m_pRWLock);

		if (iType == READ)
		{
			if (pthread_rwlock_tryrdlock((pthread_rwlock_t*)m_pRWLock) == UTIL_OK)
				return true;
		}
		else if (iType == WRITE)
		{
			if (pthread_rwlock_trywrlock((pthread_rwlock_t*)m_pRWLock) == UTIL_OK)
				return true;
		}
		return false;
	}

	Bool CRWLock::Unlock()
	{
		Assert(m_pRWLock);

		if (pthread_rwlock_unlock((pthread_rwlock_t*)m_pRWLock) == UTIL_OK)
			return true;

		return false;
	}
}
