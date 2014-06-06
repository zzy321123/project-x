#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 自旋锁操作封装														*/
	/************************************************************************/
	class UTIL_API CSpinLock : public CRefCounter
	{
	public:
		//构造
		CSpinLock();

		//析构
		virtual ~CSpinLock();

	public:
		//区域锁
		class UTIL_API Scope : public CRefCounter
		{
		public:
			Scope(CSpinLock* pSpinLock, const AString& sFile = "", Int32 iLine = 0);

			virtual ~Scope();

		protected:
			CSpinLock* m_pSpinLock;
		};

	public:
		//锁定(阻塞等待)
		virtual Bool  Lock(const AString& sFile = "", Int32 iLine = 0);

		//锁定(立即返回)
		virtual Bool  TryLock(const AString& sFile = "", Int32 iLine = 0);

		//解锁
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
