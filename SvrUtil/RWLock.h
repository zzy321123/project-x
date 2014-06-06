#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 读写锁操作封装                                                       */
	/************************************************************************/
	class UTIL_API CRWLock : public CRefCounter
	{
	public:
		//构造
		CRWLock();

		//析构
		virtual ~CRWLock();

		//锁定类型
		enum
		{
			READ  = 0x01,
			WRITE = 0x02,
		};

	public:
		//区域锁
		class UTIL_API Scope : public CRefCounter
		{
		public:
			Scope(CRWLock* pRWLock, UInt32 iType, const AString& sFile = "", Int32 iLine = 0) ;

			virtual ~Scope();

		protected:
			CRWLock*  m_pRWLock;
		};

	public:
		//锁定(阻塞等待)
		virtual Bool  Lock(UInt32 iType, const AString& sFile = "", Int32 iLine = 0);

		//锁定(立即返回)
		virtual Bool  TryLock(UInt32 iType, const AString& sFile = "", Int32 iLine = 0);

		//解锁
		virtual Bool  Unlock();

	protected:
		PVoid   m_pRWLock;
	};

	#define AutoRWLock(name, rwlock, type) CRWLock::Scope name(rwlock, type, __FILE__, __LINE__)
}
#endif
