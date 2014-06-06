#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ������������װ														*/
	/************************************************************************/
	class UTIL_API CSpinLock : public CRefCounter
	{
	public:
		//����
		CSpinLock();

		//����
		virtual ~CSpinLock();

	public:
		//������
		class UTIL_API Scope : public CRefCounter
		{
		public:
			Scope(CSpinLock* pSpinLock, const AString& sFile = "", Int32 iLine = 0);

			virtual ~Scope();

		protected:
			CSpinLock* m_pSpinLock;
		};

	public:
		//����(�����ȴ�)
		virtual Bool  Lock(const AString& sFile = "", Int32 iLine = 0);

		//����(��������)
		virtual Bool  TryLock(const AString& sFile = "", Int32 iLine = 0);

		//����
		virtual Bool  Unlock();

	protected:
		PVoid   m_pSpinLock;

#ifdef _DEBUG
		Bool    m_bLocked;
		AString m_sFile;
		Int32   m_iLine;
		Int32   m_iThread;
#endif
	};

	#define AutoSpinLock(name, spinlock) CSpinLock::Scope name(spinlock, __FILE__, __LINE__)
}
#endif
