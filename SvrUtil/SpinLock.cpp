#include "SpinLock.h"
#include "OSOperator.h"
#include "UtilMalloc.h"
#include "Exception.h"
#include "pthread.h"

namespace Util
{
	CSpinLock::Scope::Scope(CSpinLock* pSpinLock, const AString& sFile, Int32 iLine) : m_pSpinLock(pSpinLock)
	{
		if (m_pSpinLock) 
			m_pSpinLock->Lock(sFile,iLine);
	}

	CSpinLock::Scope::~Scope()
	{
		if (m_pSpinLock)
			m_pSpinLock->Unlock();
	}

	//////////////////////////////////////////////////////////////////////////

	CSpinLock::CSpinLock() : m_pSpinLock(0)
	{
		m_pSpinLock = UtilMalloc(sizeof(pthread_spinlock_t));	
		if (pthread_spin_init((pthread_spinlock_t*)m_pSpinLock, 0) != UTIL_OK)
		{
			UtilFree(m_pSpinLock);
			m_pSpinLock = 0;
		}

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif
	}

	CSpinLock::~CSpinLock()
	{
		if (m_pSpinLock)
		{
			pthread_spin_destroy((pthread_spinlock_t*)m_pSpinLock);
			UtilFree(m_pSpinLock);
			m_pSpinLock = 0;
		}
	}

	Bool CSpinLock::Lock(const AString& sFile,Int32 iLine)
	{
		Assert(m_pSpinLock);

		if (pthread_spin_lock((pthread_spinlock_t*)m_pSpinLock) == UTIL_OK)
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

	Bool CSpinLock::TryLock(const AString& sFile,Int32 iLine)
	{
		Assert(m_pSpinLock);
		if (pthread_spin_trylock((pthread_spinlock_t*)m_pSpinLock) == UTIL_OK)
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

	Bool CSpinLock::Unlock()
	{
		Assert(m_pSpinLock);

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif 

		if (pthread_spin_unlock((pthread_spinlock_t*)m_pSpinLock) == UTIL_OK)
			return true;

		return false;
	}
}
